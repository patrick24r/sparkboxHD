#include "Text.h"

Text::Text(std::string text)
{
  textString = text;
  numberOfCharacters = textString.size();
}