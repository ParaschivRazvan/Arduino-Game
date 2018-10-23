#include "LedControl.h"
#include <LiquidCrystal.h>

#define SW 2
#define X A0
#define Y A1
#define RS 1
#define E 8
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define V0 3

LiquidCrystal LCD (RS, E, D4, D5, D6, D7); //declaring the object which controls the LCD

LedControl lc = LedControl (12, 11, 10, 1); //declaring the object which controls the Matrix

struct positions {
  
  int row, col;
};

typedef struct positions positions;

positions meteors[30], projectiles[30], toDelete[30]; //the arrays that contain the meteors, projectiles and the objected to be deleted from the matrix

int  noMeteors, noProjectiles, noDelete, rowShuttle, colShuttle;// the amount of objectes within the declared arrays and the shuttle poizition 

//declaration of the variables that control the movements of each moving object 
unsigned long lastShuttleMove, lastMeteorSpawn, lastMeteorMove, lastProjectileMove, lastProjectileSpawned, score, lives;


void setup() {

  LCD.begin(16, 2);
  
  analogWrite(V0, 128);
  
  lc.shutdown(0, false);

  lc.setIntensity(0, 8);

  lc.clearDisplay(0);
  
  pinMode(SW, INPUT);

  digitalWrite(SW, HIGH);

  start();

}


void loop() {

  printInGame();
   
  deleteTool();

  updateMatrix();
    
  spawnMeteor();
   
  moveShuttle();
    
  shoot();

  if (!lives) {
    
    end();
    
    delay(500);
    
    start();
      
    }
    
 
}
void printInGame() {

  LCD.setCursor(0,0);
  LCD.print("Score: ");
  LCD.print(score);
 
   
  LCD.setCursor (0,1);
  LCD.print ("Lives: ");
  LCD.print (lives);
  
  }

void start() {
  int trig = 1;

  lc.clearDisplay(0);
  
  LCD.clear();

//setting the global variables
  lives = 3;  
  
  rowShuttle = 7;
  
  colShuttle = 3;
  
  lastMeteorSpawn = lastShuttleMove = noMeteors = noProjectiles = noDelete = lastProjectileMove = lastProjectileSpawned = score = 0 ;
  
  while (trig) {
//the start screen pattern of the matrix
    lc.setLed(0, 4, 0, true);
    lc.setLed(0, 5, 0, true);
    lc.setLed(0, 3, 0, true);
    lc.setLed(0, 4, 7, true);
    lc.setLed(0, 5, 7, true);
    lc.setLed(0, 3, 7, true);
    lc.setLed(0, 2, 0, true);
    lc.setLed(0, 2, 7, true); 
    lc.setLed(0, 1, 1, true);
    lc.setLed(0, 6, 6, true);
    lc.setLed(0, 1, 6, true);
    lc.setLed(0, 6, 1, true);
    lc.setLed(0, 0, 2, true);
    lc.setLed(0, 0, 3, true); 
    lc.setLed(0, 0, 4, true);
    lc.setLed(0, 0, 5, true);
    lc.setLed(0, 7, 2, true);
    lc.setLed(0, 7, 3, true); 
    lc.setLed(0, 7, 4, true);
    lc.setLed(0, 7, 5, true);  
    
    LCD.setCursor(0, 0);
    LCD.print("Press it...");
  
    trig = digitalRead(SW); //stops the infinite loop if the joystick is pressed
} 

  LCD.clear();
  lc.clearDisplay(0);

}

void spawnMeteor() {

 //if 1 second has passed since the last spawn, spawns a meteor 
  if (millis() - lastMeteorSpawn >= 1000) {
      
    lastMeteorSpawn = millis();
  
    positions ob;
    
    ob.row = 0;
    ob.col = random(1, 7);
    
    lc.setLed(0, ob.row, ob.col, true );

//adds the meteor to the meteors array
    addObject(ob, 1);
    
  }
   
}

//adds a meteor/projectile depending on the preset bool variable
void addObject(positions p, bool b) {
  
  if (b) {
   
    meteors[noMeteors].row = p.row;
    
    meteors[noMeteors++].col = p.col;
    
     }
      else {
      
        projectiles[noProjectiles].row = p.row;
      
        projectiles[noProjectiles++].col = p.col;
        
      }
}

//maps the positions of the joystick
int setVal(int position) {
  
  if (position >= 2 * 1023 / 3)
    
    return 1;
    
    else if (position <= 1023 / 3)
    
      return -1;
      
          else
          
            return 0;
}

//moves the shuttle
void moveShuttle() {
  
  int moveX = setVal(analogRead(X));

  if (millis() - lastShuttleMove >= 120) {

    lastShuttleMove = millis();
 
    lc.setLed(0, rowShuttle, colShuttle, false);
    lc.setLed(0, rowShuttle-1, colShuttle, false);
    lc.setLed(0, rowShuttle, colShuttle+1, false);
    lc.setLed(0, rowShuttle, colShuttle-1, false);
  
  if (colShuttle + moveX > 6)
    
    colShuttle = 6;
  
      else if (colShuttle + moveX < 1)
            
        colShuttle = 1;
            
          else 
              
            colShuttle += moveX ;

  lc.setLed(0, rowShuttle, colShuttle, true);
  lc.setLed(0, rowShuttle-1, colShuttle, true);
  lc.setLed(0, rowShuttle, colShuttle+1, true);
  lc.setLed(0, rowShuttle, colShuttle-1, true);

  }

}

void updateMatrix() {
  
//moves all the meteors if 500 ms have passed since the last move
  if (millis() - lastMeteorMove >= 500) {
    
    lastMeteorMove = millis();
    
    checkEnd(1);
    
    for ( int i = 0 ; i < noMeteors ; i++ ) {
    
      lc.setLed(0, meteors[i].row, meteors[i].col, false);
      
      meteors[i].row++;
      
      lc.setLed(0, meteors[i].row, meteors[i].col, true);

      
    }
  
  }

//moves all the projectiles if 150 ms have passed since the last move    
  if (millis() - lastProjectileMove >= 150) {
   
    lastProjectileMove = millis();
      
    checkEnd(0);    
      
   for ( int i = 0 ; i < noProjectiles ; i++ ) {
      
        lc.setLed(0, projectiles[i].row, projectiles[i].col, false);
        
        projectiles[i].row--;  
        
        lc.setLed(0, projectiles[i].row, projectiles[i].col, true);   
        
   } 
        
//deletes all the objects that have collided
    for (int i = 0 ; i < noDelete ; i++ )
   
    lc.setLed(0, toDelete[i].row, toDelete[i].col, false );

    noDelete = 0;

      
 
  }
     
}


void shoot() {
  
  int trig = digitalRead(SW);

//spawns a projectile if 300 ms have passed since the last one was spawned
  if (millis()-lastProjectileSpawned >= 300) {
  
    lastProjectileSpawned = millis();
  
    if (!trig){
    
      positions p;
    
      p.row = 5;
      p.col = colShuttle;

      lc.setLed(0, p.row, p.col, true);
   
      addObject(p, 0);//adds it to the projectiles array 
  
 
    }
  }
}

//prints the end image on the matrix
void end() {
  
  int trig = 1;
    
  LCD.clear();
   
  lc.clearDisplay(0);
  
  while (trig) {
      
    for(int i = 0 ; i < 8 ; i++ )
      
      lc.setLed(0, i, i, true);
    
    for(int i = 0 ; i < 8 ; i++ )
      
      lc.setLed(0, i, 7-i ,true);

    LCD.setCursor(0,0);
    LCD.print("Weak...");
    
    LCD.setCursor(0,1);
    LCD.print("Try again :)");
    
    trig = digitalRead(SW);
    
  
  }
  
  LCD.clear();

}

//deletes a meteor/projectile from their array depending on the bool variable
void deleteObject(int i, bool b) {
 
 if (b){
    
    meteors[i].row = meteors[noMeteors-1].row;
    
    meteors[i].col = meteors[noMeteors-1].col;

    noMeteors--;
    
  }
    
    else{
         
      projectiles[i].row = projectiles[noProjectiles-1].row;
          
      projectiles[i].col = projectiles[noProjectiles-1].col;
          
      noProjectiles--; 
          
    }

}

//verifies if the meteors/projectiles have reaches the end of the matrix and deletes them
void checkEnd(bool b){
  
  if (b) {
    
    for ( int i = 0 ; i < noMeteors ; i++ )
    
      if ( meteors[i].row == 7 ) {
    
        lc.setLed(0, meteors[i].row, meteors[i].col, false);

        deleteObject(i, 1);
      
        lives--;// a meteor has passed you..

        
      }
  }
    else {
      
      for ( int i = 0 ; i < noProjectiles ; i++ )
    
      if (projectiles[i].row == 0 ) {

        lc.setLed(0, projectiles[i].row, projectiles[i].col, false);

        deleteObject(i, 0) ;
      
          
      }
    
    
   }
  
} 

//returns a value depending of the type of colision
int checkCollision(positions p1, positions p2) {
  
  if (p1.col == p2.col && p1.row== p2.row)
        
    return 0;
        
      else if (p1.col == p2.col && p1.row+1 == p2.row)
            
        return 1;
                
  return -1;  
}



void deleteTool() {

  int i, j, k = 0;

//verifies or the possible collisions and adds the collided objects to toDelete array
//also adds the score
  for( i = 0 ; i < noMeteors ; i++ )
    for( j = 0 ; j <  noProjectiles ; j++ ) { 
    
      int colide = checkCollision( meteors[i], projectiles[j]);
    
      if (!colide) {  
      
        toDelete[k].row = meteors[i].row;
       
        toDelete[k].col = meteors[i].col;
      
        k++;
       
        noDelete++;

        score++;
      
        deleteObject(i, 1);
        
        deleteObject(j, 0);
        
      }
        else if (colide > 0){

          toDelete[k].row = meteors[i].row;
       
          toDelete[k].col = meteors[i].col;
       
          toDelete[k+1].row = projectiles[j].row;
       
          toDelete[k+1].col = projectiles[j].col;
       
          k += 2;
       
          score++;
       
          noDelete += 2;
        
          deleteObject(i, 1);
        
          deleteObject(j, 0);
       
       }
   
    }

}
