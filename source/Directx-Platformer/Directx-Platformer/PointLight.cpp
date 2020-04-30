#include "PointLight.h"

PointLight::PointLight()
	: buffer()
{
}

void PointLight::Bind()
{
	buffer.Update(pos);
	buffer.Bind();
}
