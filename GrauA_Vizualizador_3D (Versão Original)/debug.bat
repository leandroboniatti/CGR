@echo off
echo === CGR: Visualizador 3D - Debug ===
echo.

REM Verificar se o executável existe
if not exist "visualizador3d.exe" (
    echo ERRO: visualizador3d.exe nao encontrado!
    echo Execute 'make' ou compile manualmente primeiro.
    pause
    exit /b 1
)

REM Verificar se o arquivo de configuração existe
if not exist "config\scene.cfg" (
    echo AVISO: config\scene.cfg nao encontrado!
    echo Criando arquivo de configuracao basico...
    mkdir config 2>nul
    echo # Configuracao basica de debug > config\scene.cfg
    echo # Formato: obj_path pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z eliminable >> config\scene.cfg
    echo models/cube.obj 0.0 0.0 -3.0 0.0 0.0 0.0 1.0 1.0 1.0 1 >> config\scene.cfg
)

REM Verificar se existem shaders
if not exist "shaders\vertex.glsl" (
    echo AVISO: Shaders nao encontrados!
    echo O programa pode nao executar corretamente.
)

echo Arquivos verificados. Executando programa...
echo.
echo Controles:
echo - WASD ou setas: Movimento da camera
echo - Mouse: Rotacao da camera
echo - ESPACO: Disparar projetil
echo - ESC: Sair
echo.
echo Pressione qualquer tecla para continuar...
pause >nul

REM Executar o programa
echo Iniciando visualizador3d.exe...
visualizador3d.exe

REM Verificar código de saída
if %errorlevel% neq 0 (
    echo.
    echo ERRO: O programa terminou com codigo de erro %errorlevel%
    echo.
    echo Possiveis problemas:
    echo - Drivers OpenGL desatualizados
    echo - GLFW nao instalado corretamente
    echo - Arquivos de shader nao encontrados
    echo - Arquivo de configuracao invalido
    echo.
    pause
) else (
    echo.
    echo Programa executado com sucesso!
)