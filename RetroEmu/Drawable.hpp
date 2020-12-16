#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

class Drawable
{
friend class GraphixEngine;

public:
    virtual void Draw() const = 0;

};

#endif // DRAWABLE_HPP
