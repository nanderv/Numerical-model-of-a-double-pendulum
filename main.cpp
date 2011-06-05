////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#define G 9.81
#define pi 3.14159265359

using namespace std;
////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////

/* main.cpp -- A program which is able to predict a certain type of powered pendula.
  April 16, 2011
Copyright (C) 2011 Nander Voortman

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Nander Voortman nanderv@gmail.com


*/

// Timestamp, this contains all the required information for a state
struct stamp{
    double a1; // angle point 1 (absolute)
    double a2; // angle point 2 (absolute)
    double v1; // velocity point 1 (absolute)
    double v2; // Velocity point 2 (absolute)
    double l1; // Length 1 (absolute)
    double l2; // length 2 (absolute)
    double now; // Makes dif algorithm easier
    double damp; //damping const..
    double pusher;
};
// Position struct - Just for easy calculations
struct pos{
    double x1;
    double x2;
    double y1;
    double y2;
};
//Calc next frame
//d2phi/dt2 + gammadphi/dt + omega02sinphi = a sin(2pift-phi)
//a =(2pif)2 A/l
stamp calcnextStepDif(stamp now, double timestep){
    now.now=now.now+timestep;
    double f = now.v1/(2*pi);
    double A = now.l1;
    double a = (2*pi*f)*(2*pi*f)*A/now.l2;
    double omega0 = sqrt(G/now.l2);
    double acc=a*sin(2*pi*f*now.now-now.a2)-now.damp*now.v2*timestep-omega0*omega0*sin(now.a2);
    now.v2=now.v2+acc*timestep;
    now.a2=now.a2+now.v2*timestep;
    now.a1=now.a1+now.v1*timestep;
    now.v2 += (now.v1-now.v2)*now.pusher*timestep;
    return now;
}


double normangle(double ang){
while(ang>=2*pi)ang -=2*pi;
while(ang<0) ang += 2*pi;
return ang;
}

stamp calcnextStep(stamp now, double timestep){
pos pos;
now.now=now.now+timestep;
double vg=sin(now.a2)*pi*(6.02-pi)*sqrt(G/now.l2);
 now.v2-=vg*timestep;
 now.v2-=now.damp*timestep*now.v2;
 double aa=now.a2;
 now.a2+=now.v2*timestep;
 now.a1+=now.v1*timestep;
 pos.x1=now.l1*sin(now.a1-now.v1*timestep)+now.l2*sin(now.a2);
 pos.y1=now.l1*sin(now.a1-now.v1*timestep)+now.l2*sin(now.a2);
 pos.x2=now.l1*sin(now.a1);
 pos.y2=now.l1*cos(now.a1);
 double r_angle=now.v2;
 if((pos.y1-pos.y2)!= 0)
r_angle=atan((pos.x1-pos.x2)/(pos.y1-pos.y2));
else r_angle=atan((pos.x1-pos.x2)/(1e-21));

double a_angle = pi*pi*timestep*(r_angle-now.a2);
cout<<r_angle<<", "<<aa<<endl;
now.v2+=a_angle;
return now;

}

// Cout some debug values; very useful. Run program as ./Model > test.txt to store the info in test.txt
void Debug(stamp now, stamp now2){
    cout <<" "<<now.a1<<", "<<now.a2<<", "<<now.v1<<", "<<now.v2<<", "<<now2.a1<<", "<<now2.a2<<", "<<now2.v1<<", "<<now2.v2<<endl;
}

// Same as command above, but without a second debug thing
void Debug(stamp now)
{
    cout <<"A1 "<<now.a1<<", A2 "<<now.a2<<", V1 "<<now.v1<<", V2"<<now.v2<<endl;
}

int main(int argc, char *argv[]){
    stamp now;
// Filename variable
    const char* filename ;

// Did the user specify a filename?
    if(argc>1){
        filename = argv[1];

    } else
        filename = "model.txt";
    fstream settingfile;

// Read all settings from file
    char str[2000];
    settingfile.open(filename);
    settingfile>>str;
    now.a1=atof(str);
    settingfile>>str;
    now.a2=atof(str);
    settingfile>>str;
    now.v1=atof(str);
    settingfile>>str;
    now.v2=atof(str);
    settingfile>>str;
    now.l1=atof(str);
    settingfile>>str;
    now.l2=atof(str);
    settingfile>>str;
    float rate=atof(str);
    settingfile>>str;
    bool clear=false;
    bool nan=false;
    bool dif=false;
    bool exporttofile=false;
    if(atoi(str)==1)  clear=true;
        settingfile>>str;
    if(atoi(str)==1)  nan=true;
        settingfile>>str;
    if(atoi(str)==1) dif=true;
        settingfile>>str;
    if(atoi(str)==1)  exporttofile=true;
        settingfile>>str;
    double timestep=atof(str);
    cout << "timestep"<<timestep<<endl;
    settingfile>>str;
    now.damp=atof(str);
    settingfile>>str;
    now.pusher=atof(str);

// Open files to debug into.
    ofstream nanfile;
    nanfile.open ("nan.pos");
    ofstream diffile;
    diffile.open ("dif.pos");


    // cout the header of the debug table
    cout <<" now.a1, now.a2, now.v1, now.v2, now2.a1, now2.a2, now2.v1, now2.v2"<<endl;
    // Create the main window
    sf::RenderWindow App(sf::VideoMode(800, 600), "Numeriek model");

// Slow motion?
    float timespeed=1; // Slowdown (10 = ten times slower then normal)
// Calculate screen scale
    double scale = 200/(now.l1+now.l2); // Calculate screen scale
 // Copy stamp; one for each model
    stamp now2=now; // Second now (so each model starts with the same starting parameters)
    bool debug=false; // Echo the output to a command line; May be slow, but is very useful.

// Start the main loop
    while (App.IsOpened())
    {
        // Receive exit events
        sf::Event Event;
        while (App.GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
                App.Close();

        }

        // Clear screen
  if(clear)   App.Clear();

// Calculate the next steps
    for(int i=1;i<(1/timestep)/rate;i++){
        if(nan)  {      now=calcnextStep(now,timestep);

        }

    if(dif) {  now2=calcnextStepDif(now2,timestep);
           }

    }
         if(debug)   Debug(now,now2);

    // Draw the things to the screen
    if(nan){
        App.Draw(sf::Shape::Line(400, 300, 400+scale*now.l1*sin(now.a1), 300+scale*now.l1*cos(now.a1), 5, sf::Color::Red));
        App.Draw(sf::Shape::Circle(400, 300, 6, sf::Color::Yellow));
        App.Draw(sf::Shape::Line(400+scale*now.l1*sin(now.a1), 300+scale*now.l1*cos(now.a1),400+scale*now.l1*sin(now.a1)+scale*now.l2*sin(now.a2), 300+scale*now.l1*cos(now.a1)+scale*now.l2*cos(now.a2), 5, sf::Color::Green));
nanfile <<now.now<<", "<<now.l1*sin(now.a1)+now.l2*sin(now.a2)<<", "<<now.l1*cos(now.a1)+now.l2*cos(now.a2)<< endl;


        App.Draw(sf::Shape::Circle(400+scale*now.l1*sin(now.a1)+scale*now.l2*sin(now.a2), 300+scale*now.l1*cos(now.a1)+scale*now.l2*cos(now.a2), 6, sf::Color::Blue));

    }
    if(dif){
         if(exporttofile) diffile <<now2.now<<", "<<now2.l1*sin(now2.a1)+now2.l2*sin(now2.a2)<<", "<<now2.l1*cos(now2.a1)+now2.l2*cos(now2.a2)<< endl;
        App.Draw(sf::Shape::Line(400, 300, 400+scale*now2.l1*sin(now2.a1), 300+scale*now2.l1*cos(now2.a1), 5, sf::Color::Red));
        App.Draw(sf::Shape::Circle(400, 300, 6, sf::Color::Yellow));
        App.Draw(sf::Shape::Line(400+scale*now2.l1*sin(now2.a1), 300+scale*now2.l1*cos(now2.a1),400+scale*now2.l1*sin(now2.a1)+scale*now2.l2*sin(now2.a2), 300+scale*now2.l1*cos(now2.a1)+scale*now2.l2*cos(now2.a2), 5, sf::Color::Yellow));

        App.Draw(sf::Shape::Circle(400+scale*now2.l1*sin(now2.a1)+scale*now2.l2*sin(now2.a2), 300+scale*now2.l1*cos(now2.a1)+scale*now2.l2*cos(now2.a2), 6, sf::Color::Red));

    }
        // Finally, display the rendered frame on screen
        App.Display();
sf::Sleep((1/rate)*timespeed);
    }
 // The end
    cout <<" THE END; Ended after "<<now2.now/timestep<<" Frames"<< endl;
    return EXIT_SUCCESS;
}
