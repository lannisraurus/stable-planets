#include "frame.h"
#include "app.h"
#include "sys.h"

frame::frame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size) {

    // File tab
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Save, "&Save\tCtrl-S", "Save this star system");
    menuFile->Append(ID_Load, "&Load\tCtrl-L", "Load a star system");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    // Help tab
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    // Menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );

    // Status bar
    CreateStatusBar();
    SetStatusText( "Welcome to StablePlanets!" );

    // Panels
    panel = new wxPanel(this);

    // Unit lists
    timeUnits.Add("Seconds");
    timeUnits.Add("Earth Days");
    timeUnits.Add("Earth Years");
    lengthUnits.Add("Meters");
    lengthUnits.Add("Kilometers");
    lengthUnits.Add("Earth Radii");
    lengthUnits.Add("Astronomical Units");
    lengthUnits.Add("Light Years");
    lengthUnits.Add("Parsecs");
    massUnits.Add("Kilograms");
    massUnits.Add("Earth Masses");
    massUnits.Add("Solar Masses");

    // Run inteface
    run_button = new wxButton(panel,ID_Run,"RUN",wxPoint(10,10),wxSize(100,75));
    timestep_text = new wxStaticText(panel,wxID_ANY,"Time Step: ",wxPoint(120,16));
    duration_text = new wxStaticText(panel,wxID_ANY,"Duration: ",wxPoint(120,56));
    timestep_value = new wxTextCtrl(panel,ID_TimeStep,"",wxPoint(190,10),wxSize(150,30));
    duration_value = new wxTextCtrl(panel,ID_Duration,"",wxPoint(190,51),wxSize(150,30));
    timestep_units = new wxChoice(panel,ID_TimeStepUnits,wxPoint(350,10),wxSize(100,-1),timeUnits);
    timestep_units->Select(0);
    duration_units = new wxChoice(panel,ID_DurationUnits,wxPoint(350,50),wxSize(100,-1),timeUnits);
    duration_units->Select(2);
    progress_bar = new wxGauge(panel, ID_Progress, 100, wxPoint(10,100),wxSize(700,10));
    // Create planet interface
    create_planet_text = new wxStaticText(panel,wxID_ANY,"Name: ",wxPoint(460,79));
    name_value = new wxTextCtrl(panel,ID_Name,"",wxPoint(510,79),wxSize(200,20));
    create_planet_button = new wxButton(panel,ID_CreatePlanet,"New Body",wxPoint(460,10),wxSize(250,25));
    delete_planet_button = new wxButton(panel,ID_DeletePlanet,"Delete Body",wxPoint(460,40),wxSize(250,25));
    // Select planets interface
    select_planet_text = new wxStaticText(panel,wxID_ANY,"Created Planets: ",wxPoint(535,120));
    select_planet = new wxListBox(panel,ID_SelectPlanet,wxPoint(460,140),wxSize(250,100),bodyNames,wxLB_MULTIPLE);
    // Select planet center of mass interface
    select_planet_cm = new wxListBox(panel,ID_SelectPlanetsCM,wxPoint(10,140),wxSize(150,100),bodyNames,wxLB_MULTIPLE);
    cm_text = new wxStaticText(panel,wxID_ANY,"Orbits the C.M. of: ",wxPoint(30,120));
    // Distance
    select_distance = new wxStaticText(panel,wxID_ANY,"Distance: ",wxPoint(170,150));
    distance_value = new wxTextCtrl(panel,ID_Distance,"",wxPoint(240,140),wxSize(100,35));
    distance_units = new wxChoice(panel,ID_DistanceUnits,wxPoint(350,140),wxSize(100,-1),lengthUnits);
    distance_value->Enable(false);
    distance_units->Select(3);
    // Temperature
    select_temperature = new wxStaticText(panel,wxID_ANY,"Temperature [K]: ",wxPoint(10,292));
    temperature_value = new wxTextCtrl(panel,ID_Temperature,"",wxPoint(120,292),wxSize(40,20));
    select_heat_source = new wxCheckBox(panel,ID_HeatSource,"Heat Source",wxPoint(10,270));
    temperature_value->Enable(false);
    // Albedo
    // select_albedo = new wxStaticText(panel,wxID_ANY,"Albedo [unitless]: ",wxPoint(10,360));
    // albedo_value = new wxTextCtrl(panel,ID_Albedo,"",wxPoint(120,360),wxSize(40,20));
    // select_atmosphere = new wxCheckBox(panel,ID_Atmosphere,"Atmosphere [WIP]",wxPoint(10,290));
    // albedo_value->Enable(false);
    // Mass
    select_mass = new wxStaticText(panel,wxID_ANY,"Mass: ",wxPoint(170,190));
    mass_value = new wxTextCtrl(panel,ID_Mass,"",wxPoint(240,180),wxSize(100,35));
    mass_units = new wxChoice(panel,ID_MassUnits,wxPoint(350,180),wxSize(100,-1),massUnits);
    mass_units->Select(2);
    // Radius
    select_radius = new wxStaticText(panel,wxID_ANY,"Radius: ",wxPoint(170,230));
    radius_value = new wxTextCtrl(panel,ID_Radius,"",wxPoint(240,220),wxSize(100,35));
    radius_units = new wxChoice(panel,ID_RadiusUnits,wxPoint(350,220),wxSize(100,-1),lengthUnits);
    radius_units->Select(2);
    // orbit info
    select_inverted_orbit = new wxCheckBox(panel,ID_Inverted,"Inverted Orbit",wxPoint(10,250));
    // orbital speed
    select_rotation = new wxCheckBox(panel,ID_IsRotating,"Rotation",wxPoint(10,310));
    period_text = new wxStaticText(panel,wxID_ANY,"Rot.Period: ",wxPoint(170,270));
    period_value = new wxTextCtrl(panel,ID_Period,"",wxPoint(240,260),wxSize(100,35));
    period_units = new wxChoice(panel,ID_PeriodUnits,wxPoint(350,260),wxSize(100,-1),timeUnits);
    period_value->Enable(false);
    period_units->Select(1);

}

void frame::EnableDistance(wxCommandEvent& event){
  std::vector<std::string> pivots;
  wxArrayInt planetSelections;
  select_planet_cm->GetSelections(planetSelections);
  for (auto& elem: planetSelections) pivots.push_back( std::string(bodyNames[elem].mb_str()) );
  if (pivots.size()==0){
    distance_value->Enable(false);
    distance_value->ChangeValue("");
  }else{
    distance_value->Enable(true);
  }

}

void frame::EnableTemperature(wxCommandEvent& event){
  if(select_heat_source->IsChecked()){
    temperature_value->Enable(true);
  }else{
    temperature_value->Enable(false);
    temperature_value->ChangeValue("");
  }
}

void frame::EnableAtmosphere(wxCommandEvent& event){
  if(select_atmosphere->IsChecked()){
    albedo_value->Enable(true);
  }else{
    albedo_value->Enable(false);
    albedo_value->ChangeValue("");
  }
}

void frame::EnableRotation(wxCommandEvent& event){
  if(select_rotation->IsChecked()){
    period_value->Enable(true);
  }else{
    period_value->Enable(false);
    period_value->ChangeValue("");
  }
}

void frame::OnExit(wxCommandEvent& event){
    Close(true);
}

void frame::OnAbout(wxCommandEvent& event){
    wxMessageBox( "This is a program used for simulating star systems, generating useful data on planets and stars.", "About StablePLanets", wxOK | wxICON_INFORMATION );
}

void frame::OnHello(wxCommandEvent& event){
    wxLogMessage("Hi! Enjoy the program ^^!");
}

void frame::OnRun(wxCommandEvent& event){

  // needed variable
  std::stringstream analysis;
  bool valid = true;

  // timestep
  double dT;
  std::string dTs = std::string(timestep_value->GetLineText(0).mb_str());
  if (dTs=="") valid = false;
  else{
    analysis << dTs;
    analysis >> dT;
    analysis.clear();
  }
  dT *= timeSI(timestep_units->GetSelection());

  // timestep
  double T;
  std::string Ts = std::string(duration_value->GetLineText(0).mb_str());
  if (Ts=="") valid = false;
  else{
    analysis << Ts;
    analysis >> T;
    analysis.clear();
  }
  T *= timeSI(duration_units->GetSelection());

  // Validate and RUN simulation
  if (valid){

    auto run = [T,dT,this](){
      this->starSystem.solve(T,dT,this);
      this->starSystem.saveData(this);
    };

    std::thread indep{run};
    indep.detach();



  }else{
    wxMessageBox( "Something went wrong during the insertion of parameters. Check that the numbers are in a correct format.", "ERROR", wxOK | wxICON_INFORMATION );
  }

}

void frame::DeletePlanet(wxCommandEvent& event){
  wxArrayInt planetSelections;
  select_planet->GetSelections(planetSelections);
  for (auto& elem: planetSelections) {
    std::string remove = std::string(bodyNames[elem].mb_str());
    starSystem.deleteBody( remove );
    bodyNames.Remove( remove );
  }
  delete select_planet;
  delete select_planet_cm;
  select_planet = new wxListBox(panel,ID_SelectPlanet,wxPoint(460,140),wxSize(250,100),bodyNames,wxLB_MULTIPLE);
  select_planet_cm = new wxListBox(panel,ID_SelectPlanetsCM,wxPoint(10,140),wxSize(150,100),bodyNames,wxLB_MULTIPLE);
}


void frame::CreatePlanet(wxCommandEvent& event){

  // needed variable
  std::stringstream analysis;
  bool valid = true;

  // body name
  std::string planetName = std::string(name_value->GetLineText(0).mb_str());
  for (auto& elem : starSystem.getBodies()) if (planetName==elem.getName()) valid = false;
  if (planetName == "") valid = false;
  std::cout << "PLANET NAME IS " << planetName << "\n";

  // mass
  double mass;
  std::string massString = std::string(mass_value->GetLineText(0).mb_str());
  if (massString=="") valid = false;
  else{
    analysis << massString;
    analysis >> mass;
    analysis.clear();
  }
  mass *= massSI(mass_units->GetSelection());
  std::cout << "MASS IS " << mass << "\n";

  // radius
  double radius;
  std::string radiusString = std::string(radius_value->GetLineText(0).mb_str());
  if (radiusString=="") valid = false;
  else{
    analysis << radiusString;
    analysis >> radius;
    analysis.clear();
  }
  radius *= lengthSI(radius_units->GetSelection());
  std::cout << "RADIUS IS " << radius << "\n";

  // angularVelocity
  double angularVelocity = 0;
  std::string angularVelocityString = std::string(period_value->GetLineText(0).mb_str());
  if (select_rotation->IsChecked()){
    if (angularVelocityString=="") valid = false;
    else{
      analysis << angularVelocityString;
      analysis >> angularVelocity;
      analysis.clear();
    }
    angularVelocity *= timeSI(period_units->GetSelection());
    angularVelocity = (2*PI)/angularVelocity;
  }
  std::cout << "ANGULAR VELOCITY IS " << angularVelocity << "\n";

  // Temperature
  bool isHeatSource = select_heat_source->IsChecked();
  double temperature = 0;
  if (isHeatSource) {
    std::string TempString = std::string(temperature_value->GetLineText(0).mb_str());
    if (TempString=="") valid = false;
    else{
      analysis << TempString;
      analysis >> temperature;
      analysis.clear();
    }
    std::cout << "HEAT SOURCE - YES - TEMPERATURE IS " << temperature << "\n";
  } else {
    std::cout << "NOT A HEAT SOURCE\n";
  }

  // Orbit details
  bool inverted = select_inverted_orbit->IsChecked();

  // pivots
  std::vector<std::string> pivots;
  wxArrayInt planetSelections;
  select_planet_cm->GetSelections(planetSelections);
  for (auto& elem: planetSelections) pivots.push_back( std::string(bodyNames[elem].mb_str()) );
  std::cout << "PIVOTS: "; for (auto& elem: pivots) std::cout << elem << " "; std::cout << "\n";

  // body distance
  double distance = 0;
  if (pivots.size()!=0){
    std::string distanceString = std::string(distance_value->GetLineText(0).mb_str());
    if (distanceString=="") valid = false;
    else{
      analysis << distanceString;
      analysis >> distance;
      analysis.clear();
    }
    distance *= lengthSI(distance_units->GetSelection());
  }
  std::cout << "DISTANCE IS " << distance << "\n";

  // Validate and add body
  if (valid){
    starSystem.linkBody(mass,radius,distance,pivots,angularVelocity,planetName,temperature,isHeatSource,0,inverted,0);
    bodyNames.Add(planetName);
    delete select_planet;
    delete select_planet_cm;
    select_planet = new wxListBox(panel,ID_SelectPlanet,wxPoint(460,140),wxSize(250,100),bodyNames,wxLB_MULTIPLE);
    select_planet_cm = new wxListBox(panel,ID_SelectPlanetsCM,wxPoint(10,140),wxSize(150,100),bodyNames,wxLB_MULTIPLE);

    distance_value->Enable(false);
    temperature_value->Enable(false);
    albedo_value->Enable(false);
    period_value->Enable(false);

    select_atmosphere->SetValue(false);
    select_heat_source->SetValue(false);
    select_inverted_orbit->SetValue(false);
    select_rotation->SetValue(false);

    period_value->ChangeValue("");
    distance_value->ChangeValue("");
    temperature_value->ChangeValue("");
    albedo_value->ChangeValue("");
    mass_value->ChangeValue("");
    radius_value->ChangeValue("");
    name_value->ChangeValue("");

  }else{
    wxMessageBox( "Something went wrong during the insertion of parameters. Check that the numbers are in a correct format, and that the name of the body is not repeated or an empty string.", "ERROR", wxOK | wxICON_INFORMATION );
  }
}

void frame::Save(wxCommandEvent& event){

  wxFileDialog saveFileDialog(this, _("Save SYS file"), "", "", "SYS files (*.sys)|*.sys", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL) return;
  std::string path = std::string(saveFileDialog.GetPath().mb_str());
  std::cout << "Saving file " << path << "\n";
  std::ofstream output;
  output.open(path);
  std::cout << "File status: " << output.is_open() << "\n";

  for (int i = 0; i < starSystem.size(); i++){
    std::cout << "saving " << starSystem[i].getName()  <<"\n";
    output << "( NAME " << starSystem[i].getName() << " MASS " << starSystem[i].getMass()
    << " RADIUS " << starSystem[i].getRadius() << " TEMP " << starSystem[i].getTemperature()
    << " ANGLE " << starSystem[i].getAngle() << " ANGVEL " << starSystem[i].getAngularVelocity()
    << " POS " << starSystem[i].getPosition().X() << " " << starSystem[i].getPosition().Y()
    << " VEL " << starSystem[i].getVelocity().X() << " " << starSystem[i].getVelocity().Y()
    << " ACC " << starSystem[i].getAcceleration().X() << " " << starSystem[i].getAcceleration().Y()
    << " HEATSRC " << starSystem[i].getIsHeatSource() << " )"
    << "\n";
  }
  output.close();
}

void frame::Load(wxCommandEvent& event){

  starSystem = sys();
  wxFileDialog openFileDialog(this, _("Open SYS file"), "", "", "SYS files (*.sys)|*.sys", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL) return;
  std::string path = std::string(openFileDialog.GetPath().mb_str());
  std::cout << "Loading file " << path << "\n";
  std::ifstream input;
  input.open(path);
  std::cout << "File status: " << input.is_open() << "\n";

  double M, R, T, angulVel, dayAngle;
  double x,y, vx,vy, ax,ay;
  bool heatSource;
  std::string name;
  std::string in;

  while (!input.eof()){
    input >> in;
    std::cout << in << " ";
    if (in == "NAME"){
      input >> name;
    }else if(in == "MASS"){
      input >> M;
    }else if(in == "RADIUS"){
      input >> R;
    }else if(in == "TEMP"){
      input >> T;
    }else if(in == "ANGLE"){
      input >> dayAngle;
    }else if(in == "ANGVEL"){
      input >> angulVel;
    }else if(in == "POS"){
      input >> x >> y;
    }else if(in == "VEL"){
      input >> vx >> vy;
    }else if(in == "ACC"){
      input >> ax >> ay;
    }else if(in == "HEATSRC"){
      input >> heatSource;
    }else if(in == ")"){
      starSystem.linkBody( body(M,R,vec2(x,y),vec2(vx,vy),vec2(ax,ay),angulVel,name,T,heatSource,dayAngle) );
      bodyNames.Add(name);
      in = "";
    }
  }
  delete select_planet;
  delete select_planet_cm;
  select_planet = new wxListBox(panel,ID_SelectPlanet,wxPoint(460,140),wxSize(250,100),bodyNames,wxLB_MULTIPLE);
  select_planet_cm = new wxListBox(panel,ID_SelectPlanetsCM,wxPoint(10,140),wxSize(150,100),bodyNames,wxLB_MULTIPLE);
}

void frame::updateProgress(wxCommandEvent& event){
  progress_bar->SetValue( event.GetInt() );
}

void frame::updateStatus(wxCommandEvent& event){
  SetStatusText( event.GetString() );
}

wxBEGIN_EVENT_TABLE(frame, wxFrame)
  EVT_MENU(ID_Test,    frame::OnHello)
  EVT_MENU(wxID_EXIT,  frame::OnExit)
  EVT_MENU(wxID_ABOUT, frame::OnAbout)
  EVT_MENU(ID_Save, frame::Save)
  EVT_MENU(ID_Load, frame::Load)
  EVT_BUTTON(ID_Run, frame::OnRun)
  EVT_BUTTON(ID_CreatePlanet, frame::CreatePlanet)
  EVT_BUTTON(ID_DeletePlanet, frame::DeletePlanet)
  EVT_CHECKBOX(ID_HeatSource, frame::EnableTemperature)
  EVT_CHECKBOX(ID_Atmosphere, frame::EnableAtmosphere)
  EVT_CHECKBOX(ID_IsRotating, frame::EnableRotation)
  EVT_LISTBOX(ID_SelectPlanetsCM, frame::EnableDistance)
  EVT_COMMAND  (PROGRESS, wxEVT_COMMAND_TEXT_UPDATED, frame::updateProgress)
  EVT_COMMAND  (STATUS, wxEVT_COMMAND_TEXT_UPDATED, frame::updateStatus)
wxEND_EVENT_TABLE()
