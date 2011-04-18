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

Copyright (c) <YEAR>, <OWNER>
All rights reserved.

Chicken Dance License v0.1
http://supertunaman.com/cdl/

Redistribution and use in source and binary forms, with 
or without modification, are permitted provided that the 
following conditions are met:

    1. Redistributions of source code must retain the 
        above copyright notice, this list of conditions and 
        the following disclaimer.
    2. Redistributions in binary form must reproduce the 
        above copyright notice, this list of conditions and 
        the following disclaimer in the documentation and/or 
        other materials provided with the distribution.
    3. Neither the name of the <ORGANIZATION> nor the names 
        of its contributors may be used to endorse or promote 
        products derived from this software without specific 
        prior written permission.
    4. An entity wishing to redistribute in binary form or 
        include this software in their product without 
        redistribution of this software's source code with the 
        product must also submit to these conditions where 
        applicable: 
            * For every five-hundred (500) units distributed, at 
                least half of the employees or persons 
                affiliated with the product must listen to the 
                "Der Ententanz" (AKA "The Chicken Dance") as 
                composed by Werner Thomas for no less than two 
                (2) minutes
            * For every five (5) units distributed, 
                two (2) or more persons affiliated with the entity 
                must be recorded performing the full Chicken Dance, 
                in an original video at the entity's own expense,
                and a video encoded in OGG Theora format, at least 
                three (3) minutes in length, must be submitted to 
                <OWNER>, provided <OWNER>'s contact information. 
                The dance must be based upon the instructions on 
                how to do the Chicken Dance that you should have
                received with this software. If you have not 
                received instructions on how to do the Chicken
                Dance, then the dance must be chicken-like in nature.
            * Any employee or person affiliated with the product 
                must be prohibited from saying the word "plinth" in 
                public at all times, as long as distribution of the 
                product continues. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE. <ORGANIZATION> ACCEPTS NO LIABILITY FOR
ANY INJURIES OR EXPENSES SUSTAINED IN THE ACT OF FULFILLING ANY OF 
THE ABOVE TERMS AND CONDITIONS, ACCIDENTAL OR OTHERWISE.



 Nander Voortman: nanderv@gmail.com


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

/*
Everything works except for one line.. It's clearly visible which one :)
*/
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
