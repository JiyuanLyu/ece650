#include <iostream>
#include <pqxx/pqxx>

#include "exerciser.h"

using namespace std;
using namespace pqxx;

// drop and add each table to the database

// first, write a drop function if table exist
void dropTable(string table, connection * C) {
  string dropsql = "DROP TABLE IF EXISTS " + table + " CASCADE;";
  work W(*C);
  W.exec(dropsql);
  W.commit();
}

void createSTATE(connection * C) {
  string state = 
    "CREATE TABLE STATE ("
    "STATE_ID INT PRIMARY KEY,"
    "NAME VARCHAR(255) NOT NULL"
    ");";
  work W(*C);
  W.exec(state);
  W.commit();
}

void createCOLOR(connection * C) {
  string color = 
    "CREATE TABLE COLOR ("
    "COLOR_ID INT PRIMARY KEY,"
    "NAME VARCHAR(255) NOT NULL"
    ");";
  work W(*C);
  W.exec(color);
  W.commit();
}

void createTEAM(connection * C) {
  string team = 
    "CREATE TABLE TEAM ("
    "TEAM_ID INT PRIMARY KEY,"
    "NAME VARCHAR(255) NOT NULL,"
    "STATE_ID INT,"
    "COLOR_ID INT,"
    "WINS INT,"
    "LOSSES INT,"
    "FOREIGN KEY (STATE_ID) REFERENCES STATE(STATE_ID) ON DELETE SET NULL ON UPDATE CASCADE,"
    "FOREIGN KEY (COLOR_ID) REFERENCES COLOR(COLOR_ID) ON DELETE SET NULL ON UPDATE CASCADE"
    ");";
  work W(*C);
  W.exec(team);
  W.commit();
}

void createPLAYER(connection * C) {
  string player = 
    "CREATE TABLE PLAYER ("
    "PLAYER_ID INT PRIMARY KEY,"
    "TEAM_ID INT,"
    "UNIFORM_NUM INT,"
    "FIRST_NAME VARCHAR(255) NOT NULL,"
    "LAST_NAME VARCHAR(255) NOT NULL,"
    "MPG INT,"
    "PPG INT,"
    "RPG INT,"
    "APG INT,"
    "SPG DECIMAL(10,1),"
    "BPG DECIMAL(10,1),"
    "FOREIGN KEY (TEAM_ID) REFERENCES TEAM(TEAM_ID) ON DELETE SET NULL ON UPDATE CASCADE"
    ");";
  work W(*C);
  W.exec(player);
  W.commit();
}

// now insert the table
void readSTATE(connection * C) {
  // std::ifstream statefile('state.txt');
  std::ifstream statefs;
  statefs.open('state.txt', std::ifstream::in);
  string line;
  if (!statefile.is_open()) {
    cerr << "Failed to open state.txt." << endl;
  }
  while (getline(statefile, line)) {

  }
  statefile.close();
}

int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }


  //TODO: create PLAYER, TEAM, STATE, and COLOR tables in the ACC_BBALL database
  //      load each table with rows from the provided source txt files

  // the create order is: STATE, COLOR, TEAM, PLAYER for the dependent keys

  // drop table if exist before create tables, drop in the reverse order of creation
  dropTable("PLAYER", C);
  dropTable("TEAM", C);
  dropTable("STATE", C);
  dropTable("COLOR", C);

  // now create the tables by order
  createSTATE(C);
  createCOLOR(C);
  createTEAM(C);
  createPLAYER(C);

  // insert table with corresponding files

  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


