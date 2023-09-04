// wxwidgets
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/wfstream.h>

// C++ libs
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <thread>

// ROOT libraries
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TLatex.h"

// Universal constants
#define     PI                  M_PI            // unitless
#define     G                   6.6743E-11      // SI units
#define     AU                  1.50E11         // m
#define     SOLAR_MASS          1.989E30        // kg
#define     SUN_TEMPERATURE     5772            // K
#define     SUN_RADIUS          6.9634E8        // m
#define     EARTH_YEAR          3.1536E7        // s
#define     EARTH_MASS          5.97219E24      // kg
#define     EARTH_RADIUS        6.371E6         // m
#define     EARTH_DAY           86400           // s
#define     ONE_OVER_SQRT_2     0.70710678118   // unitless
#define     LIGHT_YEAR          9.4605284E15    // m
#define     PARSEC              3.08567758E16   // m

// SI CONVERT

double lengthSI(int i){
  switch(i){
    case 0: return 1; break;
    case 1: return 1000; break;
    case 2: return EARTH_RADIUS; break;
    case 3: return AU; break;
    case 4: return LIGHT_YEAR; break;
    case 5: return PARSEC; break;
    default: return 0; break;
  }
}

double timeSI(int i){
  switch(i){
    case 0: return 1; break;
    case 1: return EARTH_DAY; break;
    case 2: return EARTH_YEAR; break;
    default: return 0; break;
  }
}

double massSI(int i){
  switch(i){
    case 0: return 1; break;
    case 1: return EARTH_MASS; break;
    case 2: return SOLAR_MASS; break;
    default: return 0; break;
  }
}

// UI ID's
enum
{

    ID_Test = 1,
    ID_Run = 2,
    ID_TimeStep = 3,
    ID_Duration = 4,
    ID_TimeStepUnits = 5,
    ID_DurationUnits = 6,

    ID_Progress = 7,

    ID_CreatePlanet = 8,
    ID_DeletePlanet = 9,
    ID_SelectPlanet = 10,

    ID_SelectPlanetsCM = 11,
    ID_Distance = 12,
    ID_Mass = 13,
    ID_Radius = 14,
    ID_HeatSource = 15,
    ID_Temperature = 16,
    ID_Name = 17,
    ID_Inverted = 18,
    ID_DistanceUnits = 19,
    ID_Atmosphere = 20,
    ID_Albedo = 21,
    ID_MassUnits = 22,
    ID_RadiusUnits = 23,
    ID_Period = 24,
    ID_PeriodUnits = 25,
    ID_IsRotating = 26,
    ID_Save = 27,
    ID_Load = 28

};

///////////////////////////////////// PLANETS

// 2D std::vector class - holds basic 2D std::vector operations
class vec2{
private:
    double x,y;
public:
    vec2(): x(0), y(0) {}
    vec2(double X,double Y): x(X), y(Y) {}
    double operator* (const vec2& v) { return x*v.x + y*v.y; }
    vec2 operator+ (const vec2& v) { return vec2(x+v.x,y+v.y); }
    vec2 operator- ( const vec2& v) { return vec2(x-v.x,y-v.y); }
    friend vec2 operator* (const double& k, const vec2& v);
    vec2 operator*(const double& k) {return vec2(k*x,k*y); }
    vec2 operator/(const double& k) {return vec2(x/k,y/k); }
    double size() { return sqrt((*this)*(*this)); }
    double X() { return x; }
    double Y() { return y; }
    vec2 normalized() { double k = size(); return (1/k)*(*this) ; }
};
vec2 operator* (const double& k, const vec2& v){ return vec2(k*v.x,k*v.y); }

// Celestial Body class - holds all the information needed to describe a celestial body
class body{

private:
    // planet scalar and vector quantities
    std::string name;
    double mass, radius, temperature;
    vec2 position, velocity, acceleration;
    bool isHeatSource;
    // rotational quantities
    double angularVelocity, angle;

public:
    // initialize
    body() = default;
    body(double M, double R, vec2 pos, vec2 vel, vec2 acc, double angulVel, std::string n, double T, bool heatSource, double dayAngle)
    : mass(M), radius(R), position(pos), velocity(vel), acceleration(acc), name(n),
     angle(dayAngle), angularVelocity(angulVel), temperature(T), isHeatSource(heatSource)
    {}
    // inherent quantities
    double getMass() { return mass; }
    double getRadius() { return radius; }
    double getTemperature() { return temperature; }
    double getAngle () { return angle; }
    double getAngularVelocity() const { return angularVelocity; }
    vec2 getPosition () { return position; }
    vec2 getVelocity () { return velocity; }
    vec2 getAcceleration () { return acceleration; }
    std::string getName () { return name; }
    bool getIsHeatSource() {return isHeatSource; }
    // useful quantities
    vec2 getRotationPoint () { return position + radius*vec2(sin(angle),cos(angle));}
    double getOriginAngle() { return atan2(position.Y(),position.X()); }

};

// Planetary System - This class holds all the information about the planets and is able to create simulations.
class sys{
private:

    // Bodies
    std::vector<body> bodies;
    std::vector<body> originalBodies;
    std::vector<double> times;
    // Data
    std::vector<std::vector<std::vector<double>>> distanceToBodies;
    std::vector<std::vector<double>> temperature;
    std::vector<std::vector<double>> orbitalSpeed;
    std::vector<std::vector<double>> orbitalAccel;
    std::vector<std::vector<double>> xPositions;
    std::vector<std::vector<double>> yPositions;

public:

    // Constructors
    sys() = default;
    ~sys() = default;

    std::vector<body> getBodies(){
      return originalBodies;
    }

    body operator[](const int& i){return originalBodies[i];}

    int size () {return originalBodies.size();}

    void deleteBody(std::string name){
      std::vector<body> newBodies;
      for (int i = 0; i < originalBodies.size(); i++){
        if (originalBodies[i].getName() != name){
          newBodies.push_back(originalBodies[i]);
        }
      }
      originalBodies = newBodies;
    }

    void linkBody (body b){
      std::cout << "Linked body " << b.getName() << "\n";
      originalBodies.push_back(b);
    }

    // Adds a body to the planetary system
    void linkBody(double mass, double radius, double distance, std::vector<std::string> pivots,
      double angularVelocity, std::string name, double temperature, bool heatSource, double orbitalAngle, bool inverted, double dayAngle){
        // Find center of mass quantities
        vec2 centerOfMassPos(0,0);
        vec2 centerOfMassVel(0,0);
        double massSum = 0;
        for (auto& a : originalBodies ) for (auto& b : pivots) if (a.getName() == b) {
          centerOfMassPos = centerOfMassPos + a.getPosition()*a.getMass();
          centerOfMassVel = centerOfMassVel + a.getVelocity()*a.getMass();
          massSum += a.getMass();
        }
        if (massSum>0){
          centerOfMassPos = centerOfMassPos/massSum;
          centerOfMassVel = centerOfMassVel/massSum;
        }

        // Set body quantities - velocity and position
        double speed;
        if (distance == 0) speed = 0; else speed = abs(sqrt(G*massSum/distance));
        vec2 pos = centerOfMassPos + distance*vec2(cos(orbitalAngle),sin(orbitalAngle));
        vec2 vel = centerOfMassVel;
        if (inverted) vel = vel - speed*vec2(-sin(orbitalAngle),cos(orbitalAngle));
        else vel = vel + speed*vec2(-sin(orbitalAngle),cos(orbitalAngle));
        // Create Body
        originalBodies.push_back( body(mass, radius, pos, vel, vec2(), angularVelocity, name, temperature, heatSource, dayAngle) );

        std::cout << "Created new body: pos(" << pos.X() << " , " << pos.Y() << "), vel(" << vel.X() << " , " << vel.Y() << ")\n";
    }

    void solve(double T, double dT){
        // clean-up
        bodies = originalBodies;
        distanceToBodies.clear();
        times.clear(); temperature.clear(); orbitalSpeed.clear(); orbitalAccel.clear();
        xPositions.clear(); yPositions.clear();
        // SOLVING
        std::cout << "|| Solving system ...\n";
        // Allocate Space
        std::cout << "Allocating space\n";
        temperature.resize(bodies.size()); orbitalSpeed.resize(bodies.size()); orbitalAccel.resize(bodies.size());
        xPositions.resize(bodies.size()); yPositions.resize(bodies.size());
        distanceToBodies = std::vector<std::vector<std::vector<double>>> (bodies.size(), std::vector<std::vector<double>> (bodies.size()) ) ;
        // Trajectories
        std::cout << "Calculating trajectories\n";
        for(double t = 0; t < T; t += dT){
            std::vector<body> newBodies (bodies.size());
            std::vector<vec2> accels (bodies.size() , vec2());
            vec2 vel, pos;
            for(int i = 0; i < bodies.size(); i++){
                for(int j = i+1; j < bodies.size(); j++){
                    vec2 dist = bodies[j].getPosition() - bodies[i].getPosition();
                    vec2 r = dist.normalized();
                    double d2 = dist*dist;
                    vec2 q = (bodies[j].getMass() / d2)*r;
                    vec2 p = (bodies[i].getMass() / d2)*r;
                    accels[i] = accels[i] + q;
                    accels[j] = accels[j] - p;

                }
                // update vector quantities
                accels[i] = G*accels[i];
                vel = bodies[i].getVelocity() + dT*accels[i];
                pos = bodies[i].getPosition() + dT*vel;
                // update scalar quantities
                double temp = 0;
                if (bodies[i].getIsHeatSource()) temp = bodies[i].getTemperature();
                else{
                  for (int k = 0; k < bodies.size(); k++){
                    if(k!=i){
                      double Tk4 = pow(bodies[k].getTemperature(),4);
                      double Rk2 = bodies[k].getRadius()*bodies[k].getRadius();
                      vec2 D = bodies[i].getPosition()-bodies[k].getPosition();
                      double D2 = D*D;
                      temp += Tk4*Rk2/D2;
                    }
                  }
                  temp = pow(temp,0.25);
                  temp*=ONE_OVER_SQRT_2;
                }
                double updatedAngle = dT*bodies[i].getAngularVelocity(); while (updatedAngle>2*PI) updatedAngle-=2*PI; while (updatedAngle < 0) updatedAngle+= 2*PI;
                // update body
                newBodies[i] = body(bodies[i].getMass(),bodies[i].getRadius(),pos,vel,accels[i],bodies[i].getAngularVelocity(),bodies[i].getName(),temp,bodies[i].getIsHeatSource(),updatedAngle);
            }
            // Trajectory Update
            bodies = newBodies;
            // Data Extraction
            times.push_back(t);
            for(int i = 0; i < bodies.size(); i++){
                temperature[i].push_back( bodies[i].getTemperature() );
                orbitalSpeed[i].push_back(bodies[i].getVelocity().size());
                for(int j = 0; j < bodies.size(); j++) if(i!=j) distanceToBodies[i][j].push_back( (bodies[i].getPosition()-bodies[j].getPosition()).size() );
            }
        }
        // Data analysis from data extraction
        std::cout << "Extracting extra data\n";
        for(int i = 0; i < bodies.size(); i++){
            orbitalAccel[i].push_back(0);
            for (int j = 1; j < orbitalSpeed[i].size(); j++){
                orbitalAccel[i].push_back( (orbitalSpeed[i][j]-orbitalSpeed[i][j-1]) / dT );
            }
        }
        // DONE
        std::cout << "Done!\n";
    }

    void saveData(std::string append="", std::string time_units="s", std::string distance_units="m", double time_convert=1., double distance_convert=1.){
        // SAVING
        std::cout << "|| Saving graph data ...\n";
        // Initialize canvas and grid style
        TCanvas canvas ("","",1080,480);
        gStyle->SetGridStyle(0);
        gStyle->SetGridColor(17);
        // Converted time std::vector
        std::vector<double> T, D;
        for (const auto& i: times) T.push_back(i/time_convert);

        std::string folder = "Data";
        std::filesystem::create_directory(folder);

        // Create directories and store data
        for(int i = 0; i < bodies.size(); i++){

            std::cout << "Saving " << bodies[i].getName() << "\n";

            std::filesystem::create_directory(folder+"/"+bodies[i].getName());

            TGraph ET(T.size(),T.data(),temperature[i].data());
            ET.SetLineColor(98);
            ET.SetTitle(("Effective Temperature;time ["+time_units+"];Temperature [K]").c_str());
            ET.Draw("AL"); gPad->SetGrid();
            canvas.Modified();
            canvas.Update();
            canvas.SaveAs((folder+"/"+bodies[i].getName()+"/"+bodies[i].getName()+append+" Temperature.pdf").c_str(),"pdf");
            canvas.Clear();

            TGraph V(T.size(),T.data(),orbitalSpeed[i].data());
            V.SetLineColor(66);
            V.SetTitle(("Orbital Speed;time ["+time_units+"];velocity [ms^-1]").c_str());
            V.Draw("AL"); gPad->SetGrid();
            canvas.Modified();
            canvas.Update();
            canvas.SaveAs((folder+"/"+bodies[i].getName()+"/"+bodies[i].getName()+append+" Orbital Speed.pdf").c_str(),"pdf");
            canvas.Clear();

            TGraph A(T.size(),T.data(),orbitalAccel[i].data());
            A.SetLineColor(59);
            A.SetTitle(("Orbital Acceleration;time ["+time_units+"];acceleration [ms^-2]").c_str());
            A.Draw("AL"); gPad->SetGrid();
            canvas.Modified();
            canvas.Update();
            canvas.SaveAs((folder+"/"+bodies[i].getName()+"/"+bodies[i].getName()+append+" Orbital Acceleration.pdf").c_str(),"pdf");
            canvas.Clear();

            for (int j = 0; j < bodies.size(); j++){
              if(i!=j){
                std::vector<double> v = distanceToBodies[i][j]; for (auto& elem: v) elem*=distance_convert;
                TGraph D(T.size(),T.data(),v.data());
                D.SetLineColor(59);
                D.SetTitle(("Distance to "+bodies[j].getName()+";time ["+time_units+"];distance ["+distance_units+"]").c_str());
                D.Draw("AL"); gPad->SetGrid();
                canvas.Modified();
                canvas.Update();
                canvas.SaveAs((folder+"/"+bodies[i].getName()+"/"+bodies[i].getName()+append+" distance to "+bodies[j].getName()+".pdf").c_str(),"pdf");
                canvas.Clear();
              }
            }



        }
        // DONE
        std::cout << "Done!\n";
    }

};



///////////////////////////////////// GUI PROGRAMMING

// Application class
class app: public wxApp{
public:
    virtual bool OnInit();
};

// Frame class
class frame: public wxFrame{
public:
    frame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnRun(wxCommandEvent& event);
    void EnableTemperature(wxCommandEvent& event);
    void EnableAtmosphere(wxCommandEvent& event);
    void EnableRotation(wxCommandEvent& event);
    void CreatePlanet(wxCommandEvent& event);
    void DeletePlanet(wxCommandEvent& event);
    void EnableDistance(wxCommandEvent& event);
    void Save(wxCommandEvent& event);
    void Load(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
private:

    wxPanel* panel;

    sys starSystem;
    wxArrayString bodyNames;

    wxGauge* progress_bar;

    wxTextCtrl* albedo_value;
    wxTextCtrl* temperature_value;
    wxCheckBox* select_heat_source ;
    wxCheckBox* select_atmosphere;

    wxButton* run_button;
    wxStaticText* timestep_text;
    wxStaticText* duration_text;
    wxTextCtrl* timestep_value;
    wxTextCtrl* duration_value;
    wxChoice* timestep_units;
    wxChoice* duration_units;

    wxStaticText* create_planet_text;
    wxTextCtrl* name_value;
    wxButton* create_planet_button;
    wxButton* delete_planet_button;

    wxStaticText* select_planet_text;
    wxListBox* select_planet;

    wxListBox* select_planet_cm;
    wxStaticText* cm_text;

    wxStaticText* select_distance;
    wxTextCtrl* distance_value;
    wxChoice* distance_units;

    wxStaticText* period_text;
    wxTextCtrl* period_value;
    wxChoice* period_units;

    wxStaticText* select_temperature;

    wxStaticText* select_albedo;

    wxStaticText* select_mass;
    wxTextCtrl* mass_value;
    wxChoice* mass_units;

    wxStaticText* select_radius;
    wxTextCtrl* radius_value;
    wxChoice* radius_units;

    wxCheckBox* select_inverted_orbit;

    wxCheckBox* select_rotation;

    wxArrayString timeUnits;
    wxArrayString lengthUnits;
    wxArrayString massUnits;

};

// EVENT TABLE
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
wxEND_EVENT_TABLE()

// Implement app
wxIMPLEMENT_APP(app);

// CLASS FUNCTION IMPLEMENTATIONS

bool app::OnInit(){
    frame *f = new frame("StablePlanets",wxPoint(0,0), wxSize(720, 480));
    f->Center();
    f->Show( true );
    return true;
}

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
    select_temperature = new wxStaticText(panel,wxID_ANY,"Temperature [K]: ",wxPoint(10,340));
    temperature_value = new wxTextCtrl(panel,ID_Temperature,"",wxPoint(120,340),wxSize(40,20));
    select_heat_source = new wxCheckBox(panel,ID_HeatSource,"Heat Source",wxPoint(10,270));
    temperature_value->Enable(false);
    // Albedo
    select_albedo = new wxStaticText(panel,wxID_ANY,"Albedo [unitless]: ",wxPoint(10,360));
    albedo_value = new wxTextCtrl(panel,ID_Albedo,"",wxPoint(120,360),wxSize(40,20));
    select_atmosphere = new wxCheckBox(panel,ID_Atmosphere,"Atmosphere [WIP]",wxPoint(10,290));
    albedo_value->Enable(false);
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
    starSystem.solve(T,dT);
    starSystem.saveData();
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
    }
  }
  delete select_planet;
  delete select_planet_cm;
  select_planet = new wxListBox(panel,ID_SelectPlanet,wxPoint(460,140),wxSize(250,100),bodyNames,wxLB_MULTIPLE);
  select_planet_cm = new wxListBox(panel,ID_SelectPlanetsCM,wxPoint(10,140),wxSize(150,100),bodyNames,wxLB_MULTIPLE);
}
