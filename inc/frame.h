#ifndef __FRAME__
#define __FRAME__

#include <wx/wx.h>
#include <wx/wfstream.h>

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

#include "vec2.h"
#include "body.h"
#include "sys.h"
#include "utility.h"

// Frame class
class frame: public wxFrame{
public:
    frame(const wxString& title, const wxPoint& pos, const wxSize& size);
    wxGauge* progress_bar;
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
    void updateProgress(wxCommandEvent& event);
    void updateStatus(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
private:

    wxPanel* panel;

    sys starSystem;
    wxArrayString bodyNames;

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

#endif
