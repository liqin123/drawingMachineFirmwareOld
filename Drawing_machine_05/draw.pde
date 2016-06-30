float sholderAngleDeg = 180;
float sholderAngleRad = PI;
float elbowAngleDeg = 180;
float elbowAngleRad = PI;

PVector sholderScreenLocation = new PVector(0, 600);
PVector elbowScreenLocation = new PVector(300, 500);
PVector handScreenLocation = new PVector(300, 500);






void servoToScreenShoulder(float message) {
//message = 90;
  float m =  180 + (180 - message) ;
  
  //println("Sholder " + message + "   " + m);
  sholderAngleDeg = m ;
  sholderAngleRad = radians(m);

   
  float templengthX = cos(sholderAngleRad) * armScreenLength;
  float templengthY = sin(sholderAngleRad) * armScreenLength;

  
   //float templengthX = cos(sholderAngleRad) * 1000;
   //float templengthY = sin(sholderAngleRad) * 1000;
   
   elbowScreenLocation.x = templengthX  + sholderScreenLocation.x;
   elbowScreenLocation.y = templengthY  + sholderScreenLocation.y;

}

void servoToScreenElbow(float message) {
  //message = 90;
   float m = (message -90);//(message);
  // println("Elbow " + message+ "   " + m );
  elbowAngleDeg = m;
  elbowAngleRad = radians(m);
  
    float templengthX = cos(elbowAngleRad) * armScreenLength;
  float templengthY = sin(elbowAngleRad) * armScreenLength;
   // float templengthX = cos(elbowAngleRad) * 1000;
 // float templengthY = sin(elbowAngleRad) * 1000;
   handScreenLocation.x = templengthX  + elbowScreenLocation.x;
   handScreenLocation.y = templengthY  + elbowScreenLocation.y;
   //println("handScreenLocation" ,handScreenLocation );
}

void drawArm() {
 
  stroke(100);
  noFill();
   line(sholderScreenLocation.x, sholderScreenLocation.y, elbowScreenLocation.x, elbowScreenLocation.y);
   line( elbowScreenLocation.x, elbowScreenLocation.y, handScreenLocation.x, handScreenLocation.y);

}

void drawedges(){
   
  stroke(100);
  noFill();
  ellipseMode(CENTER);

    ellipse(sholderScreenLocation.x, sholderScreenLocation.y,  ((armScreenLength * minReach)/1000)* 2 ,  ((armScreenLength * minReach)/1000)* 2);
   ellipse(sholderScreenLocation.x, sholderScreenLocation.y,  ((armScreenLength * maxReach)/1000)* 2, ((armScreenLength * maxReach)/1000)* 2);
   
  
}