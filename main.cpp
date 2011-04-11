
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <iostream>
#define G 9.81
#define pi 3.14159265359
using namespace std;
////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////

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
    double acc=a*sin(2*pi*f*now.now-now.a2)-now.damp*now.v1*timestep-omega0*omega0*sin(now.a2);
    now.v2=now.v2+acc*timestep;
    now.a2=now.a2+now.v2*timestep;
    now.a1=now.a1+now.v1*timestep;
    return now;
}

/*
Everything works except for one line.. It's clearly visible which one :)
*/
stamp calcnextStepNan(stamp now, double timestep){
    pos p_now; // a position variable // V
    double aa = now.a2;
    double vg= sin(now.a2)*pi*pi*sqrt(G/now.l2);// V
    now.v2-=vg*timestep; // V
    now.v2 -= now.v2*timestep*now.damp;// V
    now.a2+=now.v2*timestep; // V
    p_now.x2=now.l1*sin(now.a1)+now.l2*sin(now.a2); // Old position of the middle + new position of the end V
    p_now.y2=now.l1*cos(now.a1)+now.l2*cos(now.a2); // V


    now.a1+=now.v1*timestep; // move point 1 // V

    p_now.x1=now.l1*sin(now.a1); // V
    p_now.y1=now.l1*cos(now.a1); // V
    double new_angle=atan((p_now.x2-p_now.x1)/(p_now.y2-p_now.y1)); // calculate the new angle
    now.v2 = new_angle-now.a2+(now.a2-aa)/timestep;// bug fixed, but they still behave differently.

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

int main()
{
    // Create main window
    cout <<" now.a1, now.a2, now.v1, now.v2, now2.a1, now2.a2, now2.v1, now2.v2"<<endl;
    sf::RenderWindow App(sf::VideoMode(800, 600), "Numeriek model");
    stamp now;
    now.a1=0; // Starting angle 1
    now.damp=0.0000; // Damping value
    now.a2=0.5*pi;  // Starting angle 2
    now.v1=4*2*pi;// (starting) speed 1
    now.v2=0; // (starting) speed 2
    bool nan=true; // Does Nanders Algorithm run?
    bool dif=true; // Does the differential equotation run?
    now.l1 = 0.02; // Length 1
    now.l2=0.10; // Length 2
    float rate=200; // Frames per sedond
    float timespeed=10; // Slowdown (10 = ten times slower then normal)
    bool clear=true; // Clear the screen?
    double scale = 200/(now.l1+now.l2); // Calculate screen scale
    double timestep = 0.0002; // Number of  seconds per calculated frame.
    stamp now2=now; // Second now (so each model starts with the same starting parameters)
bool debug=true; // Echo the output to a command line; May be slow, but is very useful.
// Main app loop
    while (App.IsOpened())
    {
        // Process events
        sf::Event Event;
        // We prefer programs to close if we hit the 'x'
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
        if(nan)        now=calcnextStepNan(now,timestep);


    if(dif)        now2=calcnextStepDif(now2,timestep);
    }
         if(debug)   Debug(now,now2);

    // Draw the things to the screen
    if(nan){
        App.Draw(sf::Shape::Line(400, 300, 400+scale*now.l1*sin(now.a1), 300+scale*now.l1*cos(now.a1), 5, sf::Color::Red));
        App.Draw(sf::Shape::Line(400+scale*now.l1*sin(now.a1), 300+scale*now.l1*cos(now.a1),400+scale*now.l1*sin(now.a1)+scale*now.l2*sin(now.a2), 300+scale*now.l1*cos(now.a1)+scale*now.l2*cos(now.a2), 5, sf::Color::Green));

        App.Draw(sf::Shape::Circle(400, 300, 6, sf::Color::Yellow));

        App.Draw(sf::Shape::Circle(400+scale*now.l1*sin(now.a1)+scale*now.l2*sin(now.a2), 300+scale*now.l1*cos(now.a1)+scale*now.l2*cos(now.a2), 6, sf::Color::Blue));

    }
    if(dif){


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
