/***                             GRAU A                           ***/
/*** Computação Gráfica em Tempo Real - Jogos Digitais - Unisinos ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/

/***  v:011025	***/
/*** Em parte adaptado do GRAU B de Fundamentos de CG e do Exemplo SaberThoot ***/


/*** INCLUDES ***/
#include "System.h"


/*** ENUMs e STRUCTs ***/
// ENUM para definir sentido dado pela entrada de teclado
enum sprites_states {IDLE = 1,MOVING_RIGHT,MOVING_LEFT}; // Inicializando IDLE em 1 os seguintes serão 2 e 3
enum sprites_effect	{NONE,COLLECT,DENY};

// Estrutura para armazenar informações sobre um determinado elemento Sprite
struct Sprite {	

	// Geometria e Transformações 
	GLuint VAO;     	// id do buffer de geometria	// Vertex Array Geometry do elemento
	GLuint textureID; 	// id da textura
	vec3 pos;		 	// Posição atual do elemento
	vec3 dimensions; 	// Escala aplicada ao elemento (largura, altura)
	float angle;	 	// Ângulo de rotação aplicado ao elemento	(em radianos)

	// Animação da spritesheet
	int nAnimations; 	// quantidade de linhas do spritesheet utilizado
	int nFrames;	 	// quantidade de colunas do spritesheet utilizado
	int iAnimation;  	// deslocamentos nas linhas
	int iFrame;      	// deslocamentos nas colunas
	float ds, dt;	 	// deslocamentos em x e y no spritesheet

	// movimentação do sprite
	float vel;		 	// velocidade atual do sprite

	// Para o cálculo da colisão (AABB - Axis Aligned Bounding Box) e efeito
	vec2 PMax, PMin;	// posições min e max do quadrilátero que envolve o sprite
	int effect;	// criado para informar se o sprite do tipo "item" criará dano ou coleta ao colidir com o personagem
};


/*** Protótipos (ou Cabeçalhos) das funções ***/
void key_callback (GLFWwindow *window, int key, int scancode, int action, int mode);	// Protótipo da função de callback de teclado

int setupShader (); // Função responsável pela compilação e montagem do programa de shader // Devolve o shaderID
int loadTexture (string filePath, int &width, int &height);	// Função responsável pela carga da textura // Devolve o textureID

void drawSprite (GLuint shaderID, Sprite &sprite);
void updateSprite (GLuint shaderID, Sprite &sprite);
void moveSprite (GLuint shaderID, Sprite &sprite);

//void updateItems (GLuint shader, Sprite &sprite);
void spawnItem (Sprite &sprite);	// função que gera (spawn) os itens em posições "x,y" aleatórias

void calculateAABB (Sprite &sprite);
bool checkCollision (Sprite one, Sprite two);

Sprite initializeSprite ( GLuint textureID,
						  vec3 dimensions,
						  vec3 position,
						  int effect = NONE,	// criado para informar se o sprite tipo item criará dano ou coleta ao colidir
						  int nAnimations = 1,
						  int nFrames = 1,
						  float vel = 1.5f,		// original = 1.5f
						  float angle = 0.0 );


/*** Variáveis Globais	***/
bool keys[1024];	// para identificar as teclas pressionadas

float velCharacter = MIN_VELOCITY + 0.25;	// original = 1.5f	// era float velSprites = 1.5f;
float velItems 	   = MIN_VELOCITY;	// original = 1.5f

float lastSpawnX = 400.0;	// posição "x" inicial de criação dos itens, acima da tela

float lastTime = 0;

int lives = NUM_LIVES;	// número de vidas do jogador

//int itemsTextureID[MAX_ITEMS];	// para guardar o ID das texturas de cada iten



/*** Função MAIN ***/
int main() {

	srand(time(0)); // Horário do sistema como semente para a geração de números pseudo aleatórios
	
	for (int i = 0; i < 1024; i++) { keys[i] = false; }	// Inicializando o array de controle das teclas

	System system;

	system.GLFWInit();	// GLFW e GLAD: Inicialização e configurações de versão do OpenGL

	//system.OpenGLSetup(); // Configurações adicionais da OpenGL

	system.ShaderSetup(); // Compilando e montando o programa de shader
	GLuint shaderID = system.coreShader.program; // Pegando o ID do programa de shader
	glUseProgram(shaderID); // Informa qual programa de shader será usado, no caso -> shaderID

	// Registrando a função de callback de teclado - deve ser registrada após a criação da janela	
	glfwSetKeyCallback(system.window, key_callback);

	// Criação dos sprites - objetos da cena
	Sprite background, character, coin, bomb;
	vector <Sprite> items;	// vetor para armazenamento dos itens, coins e bombs

	// Variáveis locais
	int score = 0;
	bool gameover = false;
	int imgWidth, imgHeight, textureID;

	// Carregando a textura do fundo e armazenando seu id em "backgroud"
	textureID  = loadTexture("../Textures/Backgrounds/fundo.png", imgWidth, imgHeight);
	background = initializeSprite(textureID, vec3(imgWidth * 0.4, imgHeight * 0.4, 1.0), vec3(400, 300, 0));

	// Carregando a textura do personagem e armazenando seu id em "character"
	textureID  = loadTexture("../Textures/Characters/Personagem.png", imgWidth, imgHeight);
	character  = initializeSprite(textureID, vec3(imgWidth * 3.0, imgHeight * 3.0, 1.0), vec3(400, 100, 0), NONE, SPRITE_SHEET_ROWS, SPRITE_SHEET_COLUMNS, velCharacter);

	// Carregando a textura dos itens a serem coletados (moedas) e armazenando seu id em "coin"
	textureID = loadTexture("../Textures/Items/moeda.png", imgWidth, imgHeight);
	coin = initializeSprite(textureID, vec3(imgWidth * 0.1, imgHeight * 0.1, 1.0), vec3(0, 0, 0), COLLECT);

	// Carregando a textura dos itens que causam dano (bombas) e armazenando seu id em "bomb"
	textureID = loadTexture("../Textures/Items/bomba.png", imgWidth, imgHeight);
	bomb = initializeSprite(textureID, vec3(imgWidth * 1.5, imgHeight * 1.5, 1.0), vec3(0, 0, 0), DENY);

	// inicializa e gera os primeiros itens para utilização no gameloop
	// usaremos somente dois espaços de memória (coin e bomb) para armazenamento das texturas
	// mesmo usando um número maior de itens na tela (por ex.: maxItems = 4)
	for (int i = 0; i < MAX_ITEMS; i++) {
		int n = rand() % 2;
		if (n == 1)	{ items.push_back (coin); }	// tipo COLLECT -> adiciona uma moeda
		else 		{ items.push_back (bomb); }	// tipo DENY -> adiciona uma bomba

		spawnItem(items[i]);
		//cout << "item " << i << " tipo " << n << " criado na posição " << items[i].pos.x << " , " << items[i].pos.y << " com velocidade " << items[i].vel << endl;
	}

	// Ativando o primeiro buffer de textura da OpenGL
	glActiveTexture(GL_TEXTURE0);

	// Enviar a informação de qual variável do shader armazenará o buffer da textura
	glUniform1i(glGetUniformLocation(shaderID, "textureBuffer"), 0);

	// Aplica a Matriz de projeção paralela ortográfica (usada para desenhar em 2D)
	mat4 projection = ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);  	// ortho(Left, Right, Bottom, Top, Near, Far)
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	// Matriz de modelo: transformações na geometria (objeto)
	//mat4 model = mat4(1); // matriz identidade
	//glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	// Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	// Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	character.iAnimation = IDLE; // = 1

	cout << "Pontuacao = " << score << "  Vidas: " << lives << endl;

	
	/*** Loop da aplicação - "game loop" ***/
	while (!glfwWindowShouldClose(system.window) && !gameover) {

		glfwPollEvents();	// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes

		// Limpa o buffer de cor
		glClearColor(193/255.0f, 229/255.0f, 245/255.0f, 1.0f); // define a cor de fundo - % normalizado, definido de 0.0 a 1.0 -> glClearColor(%RED, %GREEN, %BLUE, %ALPHA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Desenha o background
		glUniform2f(glGetUniformLocation(shaderID, "offsetTexture"), 0.0, 0.0); // enviando para variável uniform offsetTex
		drawSprite(shaderID, background);
		
		// Desenha o personagem
		moveSprite(shaderID, character);
		updateSprite(shaderID, character);
		drawSprite(shaderID, character);

		// Desenha os Itens
		glUniform2f(glGetUniformLocation(shaderID, "offsetTexture"), 0.0, 0.0); // enviando para variável uniform offsetTex
		for (int i = 0; i < items.size(); i++) { drawSprite (shaderID, items[i]); }

		// Checagem das colisões ou chegada ao chão

		calculateAABB(character); // Calcula (atualiza) o PMin e o PMax do personagem, usados para testar a colisão

		for (int i=0; i < items.size(); i++) {	// laço para percorrer o vetor de sprites tipo coin e bomb

			calculateAABB(items[i]);	// Calcula (atualiza) o PMin e o PMax do item armazenado em "items[i]", usados para testar a colisão

			if (checkCollision(character,items[i]))	{	// se ocorreu colisão entre personagem e qualquer dos itens

				if (items[i].effect == COLLECT)	{ // verifica se colidiu com moeda
					score++;
					if ((score % 5) == 0) {
						if (velItems < MAX_VELOCITY) { velItems += VELOCITY_INCREMENT; }	// aumenta a velocidade de queda dos itens a cada 5 pontos
						lives++;
					}
				}

				if (items[i].effect == DENY) { lives--; }	// verifica se colidiu com bomba
				
				if (lives <= 0)	{
					gameover = true;
					cout << "GAME OVER!" << endl;
				}
				else {	// gera um "novo" item em substituição ao que colidiu
					
					int n = rand() % 2;
					if (n == 1)	{ items[i] = coin; }	// tipo COLLECT -> adiciona uma moeda
					else 		{ items[i] = bomb; }	// tipo DENY -> adiciona uma bomba
				
					spawnItem(items[i]);

					cout << "Pontuacao = " << score << "  Vidas: " << lives << "  Velocidade dos Itens: " << velItems << "  Efeito do novo Item criado: " << items[i].effect  << endl;
				}
			}
			else if (items[i].pos.y > 50) {	// se não colidiu, verifica se não bateu no chão - antiga função updateItems();

				items[i].pos.y -= items[i].vel;	// atualiza a posição do item

			}	
			else {	// se não colidiu e bateu no chão, 
					
				// gera um "novo" item em substituição ao que chegou a chão
				int n = rand() % 2;
				if (n == 1)	{ items[i] = coin; }	// tipo COLLECT -> adiciona uma moeda
				else 		{ items[i] = bomb; }	// tipo DENY -> adiciona uma bomba

				spawnItem(items[i]);

				cout << "Pontuacao = " << score << "  Vidas: " << lives << "  Velocidade dos Itens: " << velItems << "  Efeito do novo Item criado: " << items[i].effect  << endl;					
			}
		}

		glBindVertexArray(0); // Desconectando o buffer de geometria

		glfwSwapBuffers(system.window);	// Troca os buffers da tela
	}  // fim do while

	// glDeleteVertexArrays(1, character.VAO); // Pede pra OpenGL desalocar os buffers

	if (!glfwWindowShouldClose(system.window)) { glfwSetWindowShouldClose(system.window, GL_TRUE); }	// para não sair da tela após o gameover

	glfwTerminate(); // Finaliza a execução da GLFW, limpando os recursos alocados por ela
	
	return 0;
} // fim do main


// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta, via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }

	if (action == GLFW_PRESS) {	keys[key] = true; }
	else if (action == GLFW_RELEASE) { keys[key] = false; }	// usamos else e não outro if por causa do deslocamento da personagem quando mantemos a tecla pressionada
}


// Função responsável pela compilação e montagem do programa de shader
// Os códigos fonte do vertex shader e do fragment shader estão nos arrays vertexShaderSource e fragmentShaderSource no iniçio desta função
// A função retorna o identificador do programa de shader (em "main" teremos shaderID = setupShader(), que equivale a shaderID = shaderProgram)
int setupShader() {	/*** Função para gerar o programa de shader ***/

	// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
	const GLchar *vertexShaderSource = R"(
		#version 400
		layout (location = 0) in vec3 coordenadasDaGeometria;
		layout (location = 1) in vec2 coordenadasDaTextura;
		uniform mat4 projection;
		uniform mat4 model;
		out vec2 textureCoord;
		void main() {
   			gl_Position = projection * model * vec4( coordenadasDaGeometria , 1.0 );
			textureCoord = vec2( coordenadasDaTextura.s , 1.0 - coordenadasDaTextura.t );
		}
	)";
		// "coordenadasDaGeometria" recebe as informações que estão no local 0 -> definidas em glVertexAttribPointer(0, xxxxxxxx);
		// "coordenadasDaTextura"   recebe as informações que estão no local 1 -> definidas em glVertexAttribPointer(1, xxxxxxxx);
		// "projection" receberá as informações da forma de projeção escolhida
		// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
		// "textureCoord" enviará ao pipeline a textura de uma posição específica
		// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado
		

	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
	const GLchar* fragmentShaderSource = R"(
		#version 400
		in vec2 textureCoord;			 // incluído
		uniform sampler2D textureBuffer; // incluído
		uniform vec2 offsetTexture;		 // incluído
		out vec4 color;
		void main() { color = texture(textureBuffer,textureCoord + offsetTexture); }	// modificado
	)";

	// Compilando os shaders
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Deletando os shaders, eles foram linkados no programa de shader e não são mais necessários
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;	// retorna o identificador para o programa de shader
}


// Função que atribui valores aos parâmetros de um determinado sprite e Cria o VAO
Sprite initializeSprite(GLuint textureID, vec3 dimensions, vec3 position, int effect, int nAnimations, int nFrames, float vel, float angle)
{
	Sprite sprite;

	sprite.textureID = textureID;
	sprite.dimensions.x = dimensions.x / nFrames;		// dimensão em "x" é 
	sprite.dimensions.y = dimensions.y / nAnimations;
	sprite.pos = position;
	sprite.effect = effect;
	sprite.nAnimations = nAnimations;
	sprite.nFrames = nFrames;
	sprite.angle = angle;
	sprite.iFrame = 0;
	sprite.iAnimation = 0;
	sprite.vel = vel;

	sprite.ds = 1.0 / (float)nFrames;		// divide pela quantidade de colunas do spritesheet
	sprite.dt = 1.0 / (float)nAnimations;	// divide pela quantidade de linhas do spritesheet

	GLfloat vertices[] = {
		// x    y    z       s          t
		// T0
		-0.5,  0.5, 0.0,    0.0,    sprite.dt,	// v0	t0
		-0.5, -0.5, 0.0,    0.0,       0.0,		// v1	t1
		 0.5,  0.5, 0.0, sprite.ds, sprite.dt, 	// v2	t2
		// T1
		-0.5, -0.5, 0.0,    0.0,       0.0,		// v1	t1
		 0.5,  0.5, 0.0, sprite.ds, sprite.dt, 	// v2	t2
		 0.5, -0.5, 0.0, sprite.ds,    0.0		// v3	t3
	};

	GLuint VBO, VAO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);
	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo)

	// Atributo posição - coord x, y, z - 3 valores
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Atributo textura - coord s, t - 2 valores
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	sprite.VAO = VAO;

	return sprite;
}


void drawSprite(GLuint shaderID, Sprite &sprite)
{
	glBindVertexArray(sprite.VAO);					// Conectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, sprite.textureID); // conectando com o buffer de textura que será usado no draw

	/*** Transformações na geometria (objeto) -> sempre na ordem Translação - Rotação - Escala ***/
	mat4 model = mat4(1); // carrega a matriz identidade na matriz de modelo 
	model = translate(model, sprite.pos); // Translação
	model = rotate(model, radians(sprite.angle), vec3(0.0, 0.0, 1.0)); // Rotação
	model = scale(model, sprite.dimensions); // Escala

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model)); // Transformações sendo enviadas para o shader através de "model"

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);			 // Desconectando ao buffer de geometria
	glBindTexture(GL_TEXTURE_2D, 0); // Desconectando com o buffer de textura
}


void updateSprite(GLuint shaderID, Sprite &sprite) {

	// Incrementando o índice do frame apenas quando fechar a taxa de FPS desejada
	float now = glfwGetTime();
	float dt = now - lastTime;
	if (dt >= 1 / FPS) {
		sprite.iFrame = (sprite.iFrame + 1) % sprite.nFrames; // incrementando ciclicamente o indice do Frame
		lastTime = now;
	}

	vec2 offsetTexture;
	offsetTexture.s = sprite.iFrame * sprite.ds;
	offsetTexture.t = sprite.iAnimation * sprite.dt;
	glUniform2f(glGetUniformLocation(shaderID, "offsetTexture"), offsetTexture.s, offsetTexture.t); // enviando cor para variável uniform "offsetTexture"
}


// Função responsável pelo carregamento da textura
int loadTexture(string filePath, int &width, int &height) {
	
	GLuint textureID; // id da textura a ser carregada

	// Gera o identificador da textura na memória
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Ajuste dos parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Carregamento da imagem usando a função stbi_load da biblioteca stb_image
	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data) {
		if (nrChannels == 3) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); }	// jpg, bmp -> 3 canais
		else { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); }				// png -> 4 canais
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else { std::cout << "Failed to load texture " << filePath << std::endl; }

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


// Função para movimentar as sprites (no caso, o personagem)
void moveSprite(GLuint shaderID, Sprite &sprite)
{
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
		sprite.pos.x -= sprite.vel;
		sprite.iAnimation = MOVING_LEFT;
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		sprite.pos.x += sprite.vel;
		sprite.iAnimation = MOVING_RIGHT;
	}

	if (!keys[GLFW_KEY_A] && !keys[GLFW_KEY_D] && !keys[GLFW_KEY_LEFT] && !keys[GLFW_KEY_RIGHT]) { sprite.iAnimation = IDLE; }
}


// função que gera (spawn) os itens a serem coletados/evitados em posições "x,y" aleatórias
void spawnItem(Sprite &sprite) {

	int max = lastSpawnX + 250;
	if (max > 790) max = 790;
	int min = lastSpawnX - 250;
	if (min < 10) min = 10;
	
	sprite.pos.x = rand() % (max - min + 1) + min;	// valor entre 10 e 790
	lastSpawnX = sprite.pos.x;
	sprite.pos.y = rand() % (1000 - 650 + 1) + 650; // valor entre 650 e 3000
	
	sprite.vel = velItems;
}


// Calcula (atualiza) o PMin e o PMax do sprite, usados para testar a colisão com outro elemento
void calculateAABB(Sprite &sprite) {

	sprite.PMin.x = sprite.pos.x - sprite.dimensions.x/2.0; 
	sprite.PMin.y = sprite.pos.y - sprite.dimensions.y/2.0; 

	sprite.PMax.x = sprite.pos.x + sprite.dimensions.x/2.0; 
	sprite.PMax.y = sprite.pos.y + sprite.dimensions.y/2.0; 
}


bool checkCollision(Sprite one, Sprite two) {

	// collision x-axis?
    bool collisionX = (one.PMax.x >= two.PMin.x) && (two.PMax.x >= one.PMin.x);

    // collision y-axis?
    bool collisionY = (one.PMax.y >= two.PMin.y) && (two.PMax.y >= one.PMin.y);

    // collision only if on both axes
    return collisionX && collisionY;
}