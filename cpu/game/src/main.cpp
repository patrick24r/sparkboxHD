#include "main.h"

// Global 
Sparkbox *spk;

int32_t main(void * arg)
{
  // Get pointer to sparkbox handle
  spk = (Sparkbox*)arg;
  // Link fatfs drivers
  spk->linkFatFs();



}
