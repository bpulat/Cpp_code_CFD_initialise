/*
CFD INPUT CALCULATOR made for EIE GROUP SRL by Baris PULAT 05/04/2019
- Revision 11/04/2019 now while saving .txt file name can be changed.
- Revision 11/04/2019 output file now has the time, date in the outpus and some bugs are fixed.
- Revision 27/05/2019 fix turbulent_dissipation_rate and turbulent_kinetic_energy and its plots, fix
plotting the values.
- Adjust all openfoam inputs
* Possible next revision adding also thermal properties. (Calculation of Nusselt Number etc..)

*/
#include <iostream>
//#include <cmath>
#include <float.h>
#include <fstream>
#include <string.h>
#include <ctime>
#include <iomanip>
#include <string>
#include <math.h>
// Domain size variables representing 3 dimension variables in meters
double domain_a,domain_b,domain_c;
// Inlet velocity value
double velocity;
// Location Altitude
double altitude,density,pressure,hydraulic_diameter,dynamic_viscosity;
// Reynolds number
double reynolds;
// Wall Space Calculation variables
double yplus_desired,yplus_resultant,friction_coefficient,wall_shear,frictional_velocity;
double first_layer_thickness_resultant, first_layer_thickness_desired;
int input = 1;
// air-data table variables
double Altitude[]={0,200,400,600,800,1000,1200,1400,1600,1800,2000,2200,
2400,2600,2800,3000,3200,3400,3600,3800,4000,4200,4400,4600,4800,5000,5200,
5400,5600,5800,6000,6200,6400,6600,6800,7000,8000,9000,10000,12000,14000,16000,18000};
double Pressure[]={101.33,98.95,96.61,94.32,92.08,89.88,87.72,85.6,83.53,81.49,79.5,
  77.55,75.63,73.76,71.92,70.12,68.36,66.63,64.94,63.28,61.66,60.07,58.52,57,55.51,
  54.05,52.62,51.23,49.86,48.52,47.22,45.94,44.69,43.47,42.27,41.11,35.65,30.8,26.5,
  19.4,14.17,10.53,7.57};
double Density[]={1.225,1.202,1.179,1.156,1.134,1.112,1.09,1.069,1.048,1.027,1.007,
  0.987,0.967,0.947,0.928,0.909,0.891,0.872,0.854,0.837,0.819,0.802,0.785,0.769,
  0.752,0.736,0.721,0.705,0.69,0.675,0.66,0.646,0.631,0.617,0.604,0.59,0.526,0.467,0.414,0.312,0.228,0.166,0.122};
double Dynamic_Viscosity[] = {1.789*0.00001,1.783*0.00001,1.777*0.00001,
1.771*0.00001,1.764*0.00001,1.758*0.00001,1.752*0.00001,1.745*0.00001,
1.739*0.00001,1.732*0.00001,1.726*0.00001,1.720*0.00001,1.713*0.00001,
1.707*0.00001,1.700*0.00001,1.694*0.00001,1.687*0.00001,1.681*0.00001,
1.674*0.00001,1.668*0.00001,1.661*0.00001,1.655*0.00001,1.648*0.00001,
1.642*0.00001,1.635*0.00001,1.628*0.00001,1.622*0.00001,1.615*0.00001,
1.608*0.00001,1.602*0.00001,1.595*0.00001,1.588*0.00001,1.582*0.00001,
1.575*0.00001,1.568*0.00001,1.561*0.00001,1.527*0.00001,1.493*0.00001,
1.458*0.00001,1.422*0.00001,1.422*0.00001,1.422*0.00001,1.422*0.00001};
double desired_permeability_perc,desired_permeability,porous_thickness;
double resistance_coefficient = 0;
int array_length  = (sizeof(Altitude) / sizeof (*Altitude));
double kinematic_viscosity;
double interp_result;
double boundary_layer_thickness,reynolds_number_for_downstream;
double turbulence_intensity,turbulence_length_scale,max_turbulence_length_scale;
int altitude_choice;
double turbulent_dissipation_rate,turbulent_kinetic_energy,turbulence_specific_dissipation_rate;
double turbulent_length_scale_commercial_softwares;
double Cmu = 0.09;        // An emprical constant specified in the turbulence model (approx. 0.09)
//Time and Date
std::string monthString[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
std::string dayString[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };


// Clear input buffer
int clear_input_buffer(void) {
    int ch;
    while (((ch = getchar()) != EOF) && (ch != '\n')) /* void */;
    return ch;
}
void getTime(int &year, int &month, int &day, int &hour, int &mins, int &secs, int &weekDay) {
  time_t tt;
  time( &tt );
  tm TM = *localtime( &tt );

  year    = TM.tm_year + 1900;
  month   = TM.tm_mon ;
  day     = TM.tm_mday;
  hour    = TM.tm_hour;
  mins    = TM.tm_min ;
  secs    = TM.tm_sec ;
  weekDay = TM.tm_wday ;
}

// Save the program input and outputs to a .txt file
void write_file() {
  std::string file_name;
  std::string str2 = ".txt";
  clear_input_buffer();
  std::cout << "Write the output file name: " ;
  std::cin >> file_name;
  std::string pathfile  = file_name + str2;
  std::ofstream myfile (pathfile.c_str());
  if (myfile.is_open()) {
    //  INPUTS
    myfile << "Inputs:" << "\n";
    myfile << "Velocity: " << velocity << " [m/s]" << "\n";
    myfile << "Domain size (X): " << domain_a << " [m]" << "\n";
    myfile << "Domain size (Y): " << domain_b << " [m]" << "\n";
    myfile << "Domain size (Z): " << domain_c << " [m]" << "\n";
    if (altitude_choice == 1) {
      myfile << "Altitude: " << altitude << " [m]" << "\n";
    }
    else if(altitude_choice ==2){
      myfile << "Density: " << density << " [kg/m^3]" << "\n";
      myfile << "Dynamic Viscosity: " << dynamic_viscosity << " [Pa*s]" << "\n";
      myfile << "Kinematic Viscosity: " << kinematic_viscosity << " [m^2/s]" << "\n";

    }
    myfile << "\n";
    //  OUTPUTS
    myfile << "Outputs:" << "\n";
    if (altitude_choice == 1) {
      myfile << "Density: " << density << " [kg/m^3]" << "\n";
      myfile << "Dynamic Viscosity: " << dynamic_viscosity << " [Pa*s]" << "\n";
      myfile << "Kinematic Viscosity: " << kinematic_viscosity << " [m^2/s]" << "\n";
      myfile << "Pressure: " << pressure << " [kPa]" << "\n";
    }
    if (reynolds != 0) {
      myfile << "Reynolds Number: " << reynolds << "\n";
    }
    if (turbulence_intensity != 0) {
      myfile << "Turbulence Intensity: " << turbulence_intensity << " [%]" <<"\n";
    }
    if (turbulence_length_scale != 0) {
      myfile << "Turbulence Length Scale: " << turbulence_length_scale << " [m]" << "\n";
      myfile << "Maximum Turbulence Length Scale: " << max_turbulence_length_scale<< " [m]" << "\n";
    }
    if (turbulent_kinetic_energy != 0){
      myfile << "Turbulent Kinetic Energy : " << turbulent_kinetic_energy << " [m^2/s^2]" << "\n";
    }
    if (turbulent_dissipation_rate !=0){
      myfile << "Turbulent Dissipation Rate : " << turbulent_dissipation_rate << " [m^2/s^3]" << "\n";
    }
    if (yplus_desired != 0){
      myfile << "\n";
      myfile << "Wall Space Calculation" << "\n";
      myfile << "Input:" << "\n";
      myfile << "Desired y+ value: " << yplus_desired << "\n";
      myfile << "Output:" << "\n";
      myfile << "Required First Layer Thickness: " << first_layer_thickness_resultant << " [m]" << "\n";
    }
    if (first_layer_thickness_desired != 0){
      myfile << "\n";
      myfile << "YPlus Calculation" << "\n";
      myfile << "Input:" << "\n";
      myfile << "Desired First Layer Thickness: " << first_layer_thickness_desired << " [m]" << "\n";
      myfile << "Output:" << "\n";
      myfile << "Y+ value: " << yplus_resultant << "\n";
    }
    if (resistance_coefficient != 0){
      myfile << "\n";
      myfile << "Permeability Calculation" << "\n";
      myfile << "Input:" << "\n";
      myfile << "Desired Permeability: " << desired_permeability_perc << " [%]" << "\n";
      myfile << "Geometry thickness: " << porous_thickness << " [m]" << "\n";
      myfile << "Output:" << "\n";
      myfile << "Resistance coefficient: " << resistance_coefficient << " [1/m]" << "\n";
    }
    myfile << "\n\n";
    int year, month, day, hour, mins, secs, weekDay;
    getTime(year, month, day, hour, mins, secs, weekDay);
    myfile << "Date: " << dayString[weekDay] << ", " << day << " " << monthString[month] << " " << year << '\n';
    #define SP << std::setfill( '0' ) << std::setw( 2 ) <<
    myfile << "Time: " SP hour << ":" SP mins << "." SP secs << '\n';
    myfile.close();
  }
  else std::cout << "Unable to open file" ;

}

void turbulence_intensity_calculator() {
  reynolds = velocity*density*hydraulic_diameter/dynamic_viscosity;
  turbulence_intensity = 0.16 * pow(reynolds,-.1/.8) * 100; // written in percentage %

}
void turbulence_length_scale_calculator() {
  reynolds_number_for_downstream = velocity*density*domain_c/dynamic_viscosity;
  boundary_layer_thickness = 0.37 * domain_c / pow(reynolds_number_for_downstream,0.2);
  turbulence_length_scale = 0.4 * boundary_layer_thickness; // Fluent Manual
  max_turbulence_length_scale = 0.07 * hydraulic_diameter;  // Wikipedia
}
void linear_interpolation(double input, double array1[], double array2[]){
  // this function takes input compares it in array1 and interpolates the data through array2
  double result;
  int position = 0;

  for (int i = 0; i < array_length; i++) {
    if (array1[i] < input) {
      position ++ ;
    }
  }
  result = ((input - array1[position-1])/(array1[position]-array1[position-1]))*(array2[position]-array2[position-1]) + array2 [position-1];
  interp_result = result;
}
void reynolds_calculator() {
reynolds = velocity*density*hydraulic_diameter/dynamic_viscosity;
if (reynolds > 500000) {
  std::cout << "Flow is TURBULENT" << "\n";
}
else{
  std::cout << "Flow is LAMINAR check your inputs carefully !";
}
}
void wall_space_calculator(){
  reynolds = velocity*density*hydraulic_diameter/dynamic_viscosity;
  std::cout << "\t\tWall Space Calculation" << "\n\n";
  std::cout << "Enter your desired y+ value: " ;
  std::cin >> yplus_desired;
  while(std::cin.fail() || yplus_desired <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter your desired y+ value: " ;
    std::cin >> yplus_desired;
  }
  friction_coefficient = 0.059/pow(reynolds,0.2);
  wall_shear = density * friction_coefficient * pow(velocity,2)/2;
  frictional_velocity = sqrt(wall_shear / density);
  first_layer_thickness_resultant = dynamic_viscosity * yplus_desired / frictional_velocity / density;
}
void turbulent_kinetic_energy_calculator(){
  turbulence_intensity_calculator();
  turbulent_kinetic_energy = 1.5 * (velocity*turbulence_intensity/100)*(velocity*turbulence_intensity/100);
}
void turbulent_dissipation_rate_calculator(){ // Also as know as epsilon in (k-e turbulence model)
  turbulence_length_scale_calculator();
  turbulent_kinetic_energy_calculator();
  turbulent_dissipation_rate = pow(Cmu,0.75)*pow(turbulent_kinetic_energy,1.5)/turbulence_length_scale;
}
void turbulence_specific_dissipation_rate_calculator(){
  // Also as known as omega in (k-w SST model w)
  turbulent_kinetic_energy_calculator();

  turbulence_specific_dissipation_rate = pow(turbulent_kinetic_energy,0.5)/(Cmu*turbulence_length_scale);


}
void turbulent_length_scale_commercial_softwares_calculator(){
//  turbulent_kinetic_energy_calculator();
//  turbulent_dissipation_rate_calculator();
  turbulent_length_scale_commercial_softwares = pow(Cmu,0.75)*pow(turbulent_kinetic_energy,1.5)/turbulent_dissipation_rate;
}
void yplus_calculator() {
  reynolds = velocity*density*hydraulic_diameter/dynamic_viscosity;
  std::cout << "\t\tY+ Calculation" << "\n\n";
  std::cout << "Enter your desired first layer thickness [m]: " ;
  std::cin >> first_layer_thickness_desired;
  while(std::cin.fail() || first_layer_thickness_desired <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter your desired first layer thickness [m]: " ;
    std::cin >> first_layer_thickness_desired;
  }
  friction_coefficient = 0.059/pow(reynolds,0.2);
  wall_shear = density * friction_coefficient * pow(velocity,2)/2;
  frictional_velocity = sqrt(wall_shear / density);
  yplus_resultant = first_layer_thickness_desired * frictional_velocity * density / dynamic_viscosity;

}
void permeability_calculator() {
  // Permeability calculation are done according to the formula of Idelchick (I.E:Idelchik, Flow Resistance, Hemisphere Publishing Corp., New York, 1989)
  std::cout << "\t\t Permeability Calculation" << "\n\n";
  std::cout << "Enter the open area in percentage [%]: ";
  std::cin >> desired_permeability_perc;
  while(std::cin.fail() || desired_permeability_perc <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter the open are in as a percentage [%]: ";
    std::cin >> desired_permeability_perc;
  }
  desired_permeability = desired_permeability_perc / 100;
  std::cout << "\n";
  std::cout << "Enter the thickness of your porous media [m]: ";
  std::cin >> porous_thickness;
  while(std::cin.fail() || porous_thickness <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter the thickness of your modeled porous media [m]: ";
    std::cin >> porous_thickness;
    }
    double f = desired_permeability;

    resistance_coefficient = (1 / pow(f,2) * pow((pow(2,0.5) / 2 *pow((1-f),0.375)+(1-f)),2)) / porous_thickness;

}
void first_inputs() {

  std::cout << "Enter your inlet velocity value [m/s]: " ;
  std::cin >> velocity;
  while(std::cin.fail() || velocity <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter your inlet velocity value [m/s]: " ;
    std::cin >> velocity;
  }
  std::cout << "Enter your domain size dimension (X) [m]: " ;
  std::cin >> domain_a;
  while(std::cin.fail() || domain_a <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter your domain size dimension (X) [m]: " ;
    std::cin >> domain_a;
  }
  std::cout << "Enter your domain size dimension (Y) [m]: " ;
  std::cin >> domain_b;
  while(std::cin.fail() || domain_b <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter your domain size dimension (Y) [m]: " ;
    std::cin >> domain_b;
  }
  std::cout << "Enter your domain size dimension (Z) [m]: " ;
  std::cin >> domain_c;
  while(std::cin.fail() || domain_c <= 0) {
    std::cout << "ERROR INPUT!" << "\n";
    std::cin.clear();
    std::cin.ignore(256,'\n');
    std::cout << "Enter your domain size dimension (Z) [m]: " ;
    std::cin >> domain_c;
  }
  hydraulic_diameter = 2*domain_a*domain_b / (domain_a+domain_b);
  std::cout << "\n\n";
  do {
    std::cout << "If your case depends on the altitude \t\t(PRESS 1) " << "\n";
    std::cout << "If you already have individual parameters \t(PRESS 2)  " << "\n\n";
    std::cout << "Your choice : " ;
    std::cin >> altitude_choice;
    while(std::cin.fail()) {
      std::cout << "*********************************" << "\n";
      std::cout << "\tError character INPUT !" << "\n";
      std::cout << "\tRE-DO YOUR CHOICE !" << "\n";
      std::cout << "*********************************" << "\n";
      std::cin.clear();
      std::cin.ignore(256,'\n');
      std::cout << "If your case depends on the altitude \t\t(PRESS 1) " << "\n";
      std::cout << "If you already have individual parameters \t(PRESS 2)  " << "\n\n";
      std::cout << "Your choice : " ;
      std::cin >> altitude_choice;
    }
    switch (altitude_choice) {
      case 1:
      std::cout << "Enter the altitude of your case [m]: ";
      std::cin >> altitude;
      while(std::cin.fail() || altitude < 0) {
        std::cout << "ERROR INPUT!" << "\n";
        std::cin.clear();
        std::cin.ignore(256,'\n');
        std::cout << "Enter the altitude of your case [m]: ";
        std::cin >> altitude;
      }
      std::cout << "\n\n";
      std::cout << "\t\t***** Dry Air Properties ******" << "\n\n";
      linear_interpolation(altitude,Altitude,Density);
      density = interp_result;
      std::cout << "Density : " << density << "[kg/m^3]" << "\n";
      linear_interpolation(altitude,Altitude,Pressure);
      pressure = interp_result;
      std::cout << "Pressure : " << pressure << "[kPa]" << "\n";
      linear_interpolation(altitude,Altitude,Dynamic_Viscosity);
      dynamic_viscosity = interp_result;
      std::cout << "Dynamic Viscosity: " << dynamic_viscosity << "[Pa*s]" << "\n";
      kinematic_viscosity = dynamic_viscosity / density;
      std::cout << "Kinematic Viscosity: " << kinematic_viscosity << "[m^2/s]" << "\n";
      break;
      case 2:
      std::cout << "Enter your density value: " ;
      std::cin >> density;
      while(std::cin.fail() || density <= 0) {
        std::cout << "ERROR INPUT!" << "\n";
        std::cin.clear();
        std::cin.ignore(256,'\n');
        std::cout << "Enter your density value: " ;
        std::cin >> density;
      }
      std::cout << "Enter your dynamic viscosity value: " ;
      std::cin >> dynamic_viscosity;
      while(std::cin.fail() || dynamic_viscosity <= 0) {
        std::cout << "ERROR INPUT!" << "\n";
        std::cin.clear();
        std::cin.ignore(256,'\n');
        std::cout << "Enter your dynamic viscosity value: " ;
        std::cin >> dynamic_viscosity;
      }
      kinematic_viscosity = dynamic_viscosity / density;
      break;
      default:
      std::cout << "*********************************" << "\n";
      std::cout << "\tWrong INPUT !" << "\n";
      std::cout << "\tSelect only 1 or 2 !" << "\n";
      std::cout << "*********************************" << "\n";
      std::cout << "\n";
      break;
    }
    }
    while((altitude_choice != 1) && (altitude_choice != 2));
}

int main() {
  int input;

  std::cout << "\t\t***** Welcome To the CFD Input Calculator v05*****" << "\n\n";
  std::cout << "- This tool is used for calculating CFD inputs with dry air properties." << "\n";
  std::cout << "- Aim of this program is to find characteristics of Turbulent Flows!" << "\n";
  std::cout << "- For the numerical inputs if you enter a character program won't work." << "\n\n";
  system("PAUSE");
  first_inputs();
  std::cout << "\n\n";

  do {
    std::cout << "\n\n";
    std::cout << "\t ***** OUTPUT SELECTION *****" << "\n\n";
    std::cout << "1. To print reynolds number" << "\n";
    std::cout << "2. To print Turbulence Intensity" << "\n";
    std::cout << "3. To print Turbulence Length Scale" << "\n";
    std::cout << "4. To print Turbulent Kinetic Energy" << "\n";
    std::cout << "5. To print Turbulent Dissipation Rate" << "\n";
    std::cout << "6. Wall Space Calculator" << "\n";
    std::cout << "7. Y+ Calculator" << "\n";
    std::cout << "8. To Change Inputs" << "\n";
    std::cout << "9. Permeability Calculator" << "\n";
    std::cout << "10. Write everything in a .txt file" << "\n\n";
    std::cout << "11. Quit" << "\n\n";
    std::cout << "Selection:  ";
    std::cin >> input;
    std::cout << "\n";

    switch (input) {
      case 1:
      reynolds_calculator();
      std::cout << "Reynolds Number : " << reynolds << "\n";
      std::cout << "\n";

      break;
      case 2:
      turbulence_intensity_calculator();
      std::cout << "Turbulence Intensity : " << turbulence_intensity << "[%]" <<"\n";
      std::cout << "\n";

      break;
      case 3:
      turbulence_length_scale_calculator();
      //turbulent_length_scale_commercial_softwares_calculator();
      std::cout << "Turbulence Length Scale : " << turbulence_length_scale << "[m]" << "\n";
      std::cout << "Maximum Turbulence Length Scale : " << max_turbulence_length_scale<< "[m]" << "\n";
      std::cout << "\n";
      //std::cout << "Turbulence Length Scale for Commercial CFD Solvers : "<< turbulent_length_scale_commercial_softwares<< "[m]" << "\n";
      //std::cout << "\n";

      break;
      case 4:
      turbulent_kinetic_energy_calculator();
      std::cout << "Turbulent Kinetic Energy : " << turbulent_kinetic_energy << "[m^2/s^2]" << "\n";
      std::cout << "\n";

      break;
      case 5:
      turbulent_dissipation_rate_calculator();
      std::cout << "Turbulent Dissipation Rate : " << turbulent_dissipation_rate << "[m^2/s^3]" << "\n";
      std::cout << "\n";

      break;
      case 6:
      wall_space_calculator();
      std::cout << "First Layer Thickness : " << first_layer_thickness_resultant << "[m]" << "\n";
      std::cout << "\n";

      break;
      case 7:
      yplus_calculator();
      std::cout << "Y+ value : " << yplus_resultant << "\n";
      std::cout << "\n";

      break;
      case 8:
      first_inputs();
      std::cout << "\n";
      break;
      case 9:
      permeability_calculator();
      std::cout << "Resistance coefficient : " << resistance_coefficient << "[1/m]" << "\n";
      std::cout << "\n";
      break;
      case 10:
      write_file();
      break;
      case 11:
      std::cout << "You have chosen Quit, Goodbye." << "\n";
      break;
      default:
      std::cout << "Bad Input !";
      break;
    }
    }
    while(input !=11);

  return 0;
}
