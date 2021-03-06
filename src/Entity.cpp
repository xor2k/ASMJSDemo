//
//  Entity.cpp
//  HumbleASMJSDemo
//
//  Created by Edward Rudd on 3/3/15.
//
//

#include "Entity.h"

Entity::Entity(TextureRef texture, const Vector2f& vector, const Rectf& bounds)
    : Sprite(texture),
    m_active(false),
    m_velocity(vector),
    m_bounds(bounds)
{
    set_center(size() / 2);
}

bool Entity::update(float delta)
{
    move_position_by(m_velocity * delta);

    if (!m_bounds.intersects(Rectf(position() - center(), size())))
    {
        m_active = false;
    }

    return m_active;
}

bool Entity::collides_with(const Entity &other)
{
    return bounds().intersects(other.bounds());
}
