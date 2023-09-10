#include "app.h"
#include "hw.h"

int main() {
  hw_init();
  app_init();

  app_main();

  return 0;
}
