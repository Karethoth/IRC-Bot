#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "../extension.hpp"


class BotExtension : public ExtensionBase
{
  
};


extern "C"
{
  ExtensionBase *GetExtension()
  {
    return new BotExtension;
  }

  void *DestroyExtension( ExtensionBase *extension )
  {
    delete extension;
  }
}

#endif

