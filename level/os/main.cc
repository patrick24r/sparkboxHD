#include <cstdlib>
#include <iostream>
#include <string>

#include "sparkbox/sparkbox.h"
#include "sparkbox/task.h"
#include "sparkbox_device.h"

namespace {

using ::sparkbox::Sparkbox;

}

namespace sparkbox::level::os {

void EntryTask(void* data) {
  Sparkbox* sbox = reinterpret_cast<Sparkbox*>(data);
  SP_LOG_INFO("Playing gettysburg.wav...");
  sbox->Audio().SetChannelAudioSource(0, "sounds/gettysburg.wav");
  sbox->Audio().PlayAudio(0, -1);

  while (1) {
    taskYIELD();
  }
}

}  // namespace sparkbox::level::os

int main(void) {
  Sparkbox sbox = device::GetSparkbox();
  sbox.SetUp();

  // Set the main level task
  sparkbox::Task entry_task =
      sparkbox::Task("os_entry", sparkbox::level::os::EntryTask);
  entry_task.AddToScheduler(&sbox);

  // Start the sparkbox
  sbox.Start();

  sbox.TearDown();
  return 0;
}