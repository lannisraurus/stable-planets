#include "app.h"
#include "frame.h"

wxIMPLEMENT_APP(app);

bool app::OnInit(){
    frame *f = new frame("StablePlanets",wxPoint(0,0), wxSize(720, 480));
    f->Center();
    f->Show( true );
    return true;
}
