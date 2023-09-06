#include "sys.h"
#include "frame.h"
#include "app.h"

///////////////////////////////////// SOLVE

void sys::deleteBody(std::string name){
  std::vector<body> newBodies;
  for (int i = 0; i < originalBodies.size(); i++){
    if (originalBodies[i].getName() != name){
      newBodies.push_back(originalBodies[i]);
    }
  }
  originalBodies = newBodies;
}

void sys::linkBody (body b){
  std::cout << "Linked body " << b.getName() << "\n";
  originalBodies.push_back(b);
}

void sys::linkBody(double mass, double radius, double distance, std::vector<std::string> pivots,
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

DECLARE_APP(app)
void sys::solve(double T, double dT, frame* f){

    // events
    wxCommandEvent prog( wxEVT_COMMAND_TEXT_UPDATED, PROGRESS );
    wxCommandEvent stat( wxEVT_COMMAND_TEXT_UPDATED, STATUS );
    int val;
    std::string msg;

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


        val = round(f->progress_bar->GetRange()*float(t/T));
        prog.SetInt( val );
        f->GetEventHandler()->AddPendingEvent(prog);
        msg = "Calculating Trajectories... ("+std::to_string(val)+" %)";
        stat.SetString( msg );
        f->GetEventHandler()->AddPendingEvent(stat);

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

    val = 0;
    prog.SetInt( val );
    f->GetEventHandler()->AddPendingEvent(prog);
    msg = "Done!";
    stat.SetString( msg );
    f->GetEventHandler()->AddPendingEvent(stat);


    // Data analysis from data extraction
    std::cout << "Extracting extra data\n";
    int BSN = bodies.size();
    for(int i = 0; i < BSN; i++){

        val = round(f->progress_bar->GetRange()*float(i)/float(BSN));
        prog.SetInt( val );
        f->GetEventHandler()->AddPendingEvent(prog);
        msg = "Extracting additional data... ("+std::to_string(val)+" %)";
        stat.SetString( msg );
        f->GetEventHandler()->AddPendingEvent(stat);

        orbitalAccel[i].push_back(0);
        for (int j = 1; j < orbitalSpeed[i].size(); j++){
            orbitalAccel[i].push_back( (orbitalSpeed[i][j]-orbitalSpeed[i][j-1]) / dT );
        }
    }

    val = 0;
    prog.SetInt( val );
    f->GetEventHandler()->AddPendingEvent(prog);
    msg = "Done!";
    stat.SetString( msg );
    f->GetEventHandler()->AddPendingEvent(stat);

    // DONE
    std::cout << "Done!\n";
}

void sys::saveData(frame* f, std::string append, std::string time_units, std::string distance_units, double time_convert, double distance_convert){

    // events
    wxCommandEvent prog( wxEVT_COMMAND_TEXT_UPDATED, PROGRESS );
    wxCommandEvent stat( wxEVT_COMMAND_TEXT_UPDATED, STATUS );
    int val;
    std::string msg;

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

        float ratio = (float(i)+1)/float(bodies.size());
        val = f->progress_bar->GetRange()*ratio;
        prog.SetInt( val );
        f->GetEventHandler()->AddPendingEvent(prog);
        msg = "Saving data on "+bodies[i].getName()+" ("+std::to_string(val)+" %)";
        stat.SetString( msg );
        f->GetEventHandler()->AddPendingEvent(stat);

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

    val = 0;
    prog.SetInt( val );
    f->GetEventHandler()->AddPendingEvent(prog);
    msg = "Done!";
    stat.SetString( msg );
    f->GetEventHandler()->AddPendingEvent(stat);

    // DONE
    std::cout << "Done!\n";
}
