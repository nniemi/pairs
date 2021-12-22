#include "card.hh"
#include <iostream>

Card::Card():
    letter_(EMPTY_CHAR), visibility_(EMPTY)
{

}

Card::Card(const char c):
    letter_(c), visibility_(HIDDEN)
{

}

void Card::setLetter(const char c)
{
    letter_ = c;
}

void Card::setVisibility(const Visibility_type visibility)
{
    visibility_ = visibility;
}

char Card::getLetter() const
{
    return letter_;
}

Visibility_type Card::getVisibility() const
{
    return visibility_;
}

void Card::turn()
{
    if(visibility_ == HIDDEN)
    {
        visibility_ = OPEN;
    }
    else if(visibility_ == OPEN)
    {
        visibility_ = HIDDEN;
    }
    else // if(visibility_ == EMPTY)
    {
        std::cout << "Cannot turn an empty place." << std::endl;
    }
}

void Card::print() const
{
    if (visibility_== HIDDEN) {
        std::cout<<HIDDEN_CHAR;
    }
    else if (visibility_== EMPTY) {
            std::cout<<EMPTY_CHAR;
    } else {
        std::cout<<letter_;
    }

}

void Card::removeFromGameBoard()
{
    visibility_ = EMPTY;
}
