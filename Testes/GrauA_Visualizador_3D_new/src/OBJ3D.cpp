#include "OBJ3D.h"

void BoundingBox::calculate(const std::vector<glm::vec3>& vertices) {
    if (vertices.empty()) {
        min = max = center = glm::vec3(0.0f);
        radius = 0.0f;
        return;
    }

    min = max = vertices[0];
    
    for (const auto& vertex : vertices) {
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
    
    center = (min + max) * 0.5f;
    radius = glm::distance(center, max);
}

bool BoundingBox::intersects(const BoundingBox& other) const {
    return glm::distance(center, other.center) <= (radius + other.radius);
}

bool BoundingBox::intersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    // Implementação de intersecção raio-esfera
    glm::vec3 oc = rayOrigin - center;
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(oc, rayDirection);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) {
        return false;
    }
    
    float t1 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + sqrt(discriminant)) / (2.0f * a);
    
    distance = (t1 > 0) ? t1 : t2;
    return distance > 0;
}

OBJ3D::OBJ3D() 
    : transformMatrix(1.0f)
    , position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
    , eliminable(true)
    , visible(true) {
}

OBJ3D::OBJ3D(const std::string& objFilename)
    : filename(objFilename)
    , transformMatrix(1.0f)
    , position(0.0f)
    , rotation(0.0f)
    , scale(1.0f)
    , eliminable(true)
    , visible(true) {
}

OBJ3D::~OBJ3D() {
    cleanup();
}

const std::string& OBJ3D::getFilename() const {
    return filename;
}

const std::vector<glm::vec3>& OBJ3D::getVertices() const {
    return vertices;
}

const std::vector<glm::vec2>& OBJ3D::getTextureCoords() const {
    return textureCoords;
}

const std::vector<glm::vec3>& OBJ3D::getNormals() const {
    return normals;
}

const std::vector<std::shared_ptr<Group>>& OBJ3D::getGroups() const {
    return groups;
}

const glm::mat4& OBJ3D::getTransformMatrix() const {
    return transformMatrix;
}

const glm::vec3& OBJ3D::getPosition() const {
    return position;
}

const glm::vec3& OBJ3D::getRotation() const {
    return rotation;
}

const glm::vec3& OBJ3D::getScale() const {
    return scale;
}

const BoundingBox& OBJ3D::getBoundingBox() const {
    return boundingBox;
}

bool OBJ3D::isEliminable() const {
    return eliminable;
}

bool OBJ3D::isVisible() const {
    return visible;
}

void OBJ3D::setFilename(const std::string& objFilename) {
    filename = objFilename;
}

void OBJ3D::setPosition(const glm::vec3& pos) {
    position = pos;
    updateTransformMatrix();
}

void OBJ3D::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateTransformMatrix();
}

void OBJ3D::setScale(const glm::vec3& scl) {
    scale = scl;
    updateTransformMatrix();
}

void OBJ3D::setEliminable(bool elim) {
    eliminable = elim;
}

void OBJ3D::setVisible(bool vis) {
    visible = vis;
}

void OBJ3D::addVertex(const glm::vec3& vertex) {
    vertices.push_back(vertex);
}

void OBJ3D::addTextureCoord(const glm::vec2& texCoord) {
    textureCoords.push_back(texCoord);
}

void OBJ3D::addNormal(const glm::vec3& normal) {
    normals.push_back(normal);
}

void OBJ3D::addGroup(std::shared_ptr<Group> group) {
    groups.push_back(group);
}

void OBJ3D::translate(const glm::vec3& offset) {
    position += offset;
    updateTransformMatrix();
}

void OBJ3D::rotate(const glm::vec3& angles) {
    rotation += angles;
    updateTransformMatrix();
}

void OBJ3D::scaleObject(const glm::vec3& factors) {
    scale *= factors;
    updateTransformMatrix();
}

void OBJ3D::updateTransformMatrix() {
    transformMatrix = glm::mat4(1.0f);
    transformMatrix = glm::translate(transformMatrix, position);
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transformMatrix = glm::rotate(transformMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    transformMatrix = glm::scale(transformMatrix, scale);
}

void OBJ3D::setupVAOs() {
    for (auto& group : groups) {
        group->setupVAO(vertices, textureCoords, normals);
    }
}

void OBJ3D::render() {
    if (!visible) return;
    
    for (auto& group : groups) {
        group->render();
    }
}

void OBJ3D::cleanup() {
    for (auto& group : groups) {
        group->cleanup();
    }
}

void OBJ3D::calculateBoundingBox() {
    boundingBox.calculate(vertices);
}

bool OBJ3D::checkCollision(const OBJ3D& other) const {
    return boundingBox.intersects(other.boundingBox);
}

bool OBJ3D::checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    return boundingBox.intersectsRay(rayOrigin, rayDirection, distance);
}