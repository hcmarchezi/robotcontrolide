
#include "Interval.h"

#include <wx/wx.h>

void Interval::Wait(int seconds)
{
  wxSleep(seconds);

  //// Espera 10 segundos
  //int seconds = wxDateTime::GetSecond();
  //int future_seconds = seconds + 20;
  //while ( seconds < future_seconds )
  //{
  //  seconds = wxDateTime::GetSecond();
  //}
}
