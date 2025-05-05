#include "engine/light.hpp"

light::light(GLenum light, unsigned char type, vector3 position, vector3 direction, float cutoff)
{
    this->_light = light;
    this->type = type;
    this->position = position;
    this->direction = direction;
    this->cutoff = cutoff;

    std::stringstream ss;
    ss << "Light of type " << this->type << " created. Pos: " << this->position << " | Dir: " << this->direction << " | Cutoff: " << cutoff;
    printer::print_info(ss.str());

    static float white[] = {1.0f, 1.0f, 1.0f, 1.0f};
    static float lgrey[] = {0.8f, 0.8f, 0.8f, 1.0f};
    static float dgrey[] = {0.2f, 0.2f, 0.2f, 1.0f};
    static float black[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glEnable(this->_light);

    glLightfv(this->_light, GL_AMBIENT, dgrey);
    glLightfv(this->_light, GL_DIFFUSE, white);
    glLightfv(this->_light, GL_SPECULAR, white);

    static float const_atenuation = 1.0f;
    static float linear_atenuation = 0.1f;
    static float quadratic_atenuation = 0.01f;

    static float spot_exponent = 10.0f;

    if (type == 'p' || type == 's')
    {
        glLightf(this->_light, GL_CONSTANT_ATTENUATION, const_atenuation);
        glLightf(this->_light, GL_LINEAR_ATTENUATION, linear_atenuation);
        glLightf(this->_light, GL_QUADRATIC_ATTENUATION, quadratic_atenuation);
    }
    if (type == 's')
    {
        glLightf(this->_light, GL_SPOT_CUTOFF, cutoff);
        glLightf(this->_light, GL_SPOT_EXPONENT, spot_exponent);
    }
}

void light::apply_light()
{
    float pos_or_dir[4];
    float spot_dir[3];
    if (type == 'p' || type == 's')
    {
        pos_or_dir[0] = this->position.x;
        pos_or_dir[1] = this->position.y;
        pos_or_dir[2] = this->position.z;
        pos_or_dir[3] = 1.0f;
    }
    else if (type == 'd')
    {
        pos_or_dir[0] = this->direction.x;
        pos_or_dir[1] = this->direction.y;
        pos_or_dir[2] = this->direction.z;
        pos_or_dir[3] = 0.0f;
    }

    glLightfv(this->_light, GL_POSITION, pos_or_dir);
    if (type == 's')
    {
        spot_dir[0] = this->direction.x;
        spot_dir[1] = this->direction.y;
        spot_dir[2] = this->direction.z;

        glLightfv(this->_light, GL_SPOT_DIRECTION, spot_dir);
    }
}