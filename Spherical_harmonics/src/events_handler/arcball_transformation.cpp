#include "arcball_transformation.h"

arcball_transformation::arcball_transformation()
{
	// Empty constructor
}

arcball_transformation::~arcball_transformation()
{
	// Empty destructor
}

void arcball_transformation::OnMouseDown(glm::vec2 mousePt)
{
    // Get the last transformation befor starting the new rotation transformation
    this->lastTransformation = this->rotationQuaternion;

    // Start the new transformation
	this->startVector = ProjectToSphere(mousePt, 2.0f);
}

void arcball_transformation::OnMouseMove(glm::vec2 mousePt)
{
    this->endVector = ProjectToSphere(mousePt, 2.0f);

    // Compute the rotation quaternion from the start and end vectors
    glm::vec3 perp = glm::cross(startVector, endVector);
    glm::quat q;

    if (perp.length() > 0.000001f)
    {
        // Perpendicular vector as transform
        q = glm::normalize(glm::quat(glm::dot(startVector, endVector),perp.x, perp.y, perp.z));
    }
    else
    {
        // No rotation 
        q = glm::quat(1.0f, 0.0, 0.0, 0.0);
    }

       // Update the Transformation
    this->rotationQuaternion =  q * lastTransformation;
}

void arcball_transformation::OnMouseUp(glm::vec2 mousePt)
{
    // Mouse Up - Rotation ends
}


glm::vec3 arcball_transformation::ProjectToSphere(glm::vec2 point, float radius)
{
    // Scale point to range [-1,1]
    float x = point.x / radius;
    float y = point.y / radius;

    // Compute square of the length of the vector from this point to the center
    float quadrance = (x * x) + (y * y);

    if (quadrance > 1.0f)
    {
        // Point is outside the sphere, project onto the sphere surface
        float norm = 1.0f / (float)std::sqrt(quadrance);
        return glm::vec3(x * norm, y * norm, 0.0f);
    }
    else
    {
        // Point is inside the sphere, compute z coordinate
        float z = (float)std::sqrt(1.0f - quadrance);
        return glm::vec3(x, y, z);
    }
}

void arcball_transformation::setDefault(const int& viewType)
{
    if (viewType == 1)
    {
        // Isometric 1
        this->rotationQuaternion = glm::quat(0.4402697668541200f, 0.8215545196058330f, 0.2968766167094340f, -0.2075451231915790f);
    }
    else if (viewType == 2)
    {
        // Top view
        this->rotationQuaternion = glm::quat( 0.707107f,0.707107f, 0.0f, 0.0f);
           
    }
    else if (viewType == 3)
    {
        // Bottom view
        this->rotationQuaternion = glm::quat(-0.707107f,0.707107f, 0.0f, 0.0f );
           
    }
    else if (viewType == 4)
    {
        // Front view
        this->rotationQuaternion = glm::quat(1.0f,0.0f, 0.0f, 0.0f);
           
    }
    else if (viewType == 5)
    {
        // Back view
        this->rotationQuaternion = glm::quat(0.0f,0.0f, 1.0f, 0.0f);
           
    }
    else if (viewType == 6)
    {
        // Right view
        this->rotationQuaternion = glm::quat(0.5f,0.5f, 0.5f, -0.5f);
           
    }
    else if (viewType == 7)
    {
        // Left view
        this->rotationQuaternion = glm::quat(0.5f,0.5f, -0.5f, 0.5f);
    }

   //     Isometric view 1: ( 0.5950110401958050f, 0.3823692231254420f, 0.6691471521246630f, -0.2280301033964720f)
   //     Isometric view 2 : (0.3823692231254420f, 0.6691471521246630f, 0.2280301033964720f, -0.5950110401958050f)
   //     Isometric view 3 : (0.6691471521f, 0.2280301034f, -0.5950110402f, -0.3823692231f)
   //     Isometric view 4 : (0.5950110402f, -0.3823692231f, -0.6691471521f, 0.2280301034f)
   //     Isometric view 5 : (0.3823692231f, -0.6691471521f, -0.2280301034f, 0.5950110402f)
   //     Isometric view 6 : (0.6691471521f, -0.2280301034f, 0.5950110402f, 0.3823692231f)

}

glm::mat4 arcball_transformation::getRotationMatrix()
{
    // Important flip the rotation because the view direction and ligt direction are -1 Z-direction
    glm::mat4 flipMatrix = glm::mat4(1.0f);
    flipMatrix[0][0] = -1.0f; // To flip the X-axis (left-right)
    flipMatrix[1][1] = -1.0f; // To flip the Y-axis (top-bottom)


    glm::mat4 rotationMatrix = glm::mat4_cast(this->rotationQuaternion);
    glm::mat4 correctedRotation = flipMatrix * rotationMatrix; // Apply the flip correction


    return correctedRotation;
}
