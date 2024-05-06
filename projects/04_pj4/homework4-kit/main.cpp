#include <iostream>
#include <pqxx/pqxx>
#include <fstream>
#include <string>

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
    "STATE_ID SERIAL PRIMARY KEY,"
    "NAME VARCHAR(255) NOT NULL"
    ");";
  work W(*C);
  W.exec(state);
  W.commit();
}

void createCOLOR(connection * C) {
  string color = 
    "CREATE TABLE COLOR ("
    "COLOR_ID SERIAL PRIMARY KEY,"
    "NAME VARCHAR(255) NOT NULL"
    ");";
  work W(*C);
  W.exec(color);
  W.commit();
}

void createTEAM(connection * C) {
  string team = 
    "CREATE TABLE TEAM ("
    "TEAM_ID SERIAL PRIMARY KEY,"
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
    "PLAYER_ID SERIAL PRIMARY KEY,"
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
  std::ifstream statefs("state.txt");
  string line;
  if (!statefs.is_open()) {
    cerr << "Failed to open state.txt." << endl;
  }

  while (getline(statefs, line)) {
    int state_id;
    string name;
    stringstream ss(line);
    ss >> state_id >> name;
    // cout << "State Name: " << name << ", State ID: " << state_id << endl;
    add_state(C, name);
  }

  statefs.close();
}

void readCOLOR(connection * C) {
  std::ifstream colorfs("color.txt");
  string line;
  if (!colorfs.is_open()) {
    cerr << "Failed to open color.txt." << endl;
  }

  while (getline(colorfs, line)) {
    int color_id;
    string name;
    stringstream ss(line);
    ss >> color_id >> name;
    // cout << "Color Name: " << name << ", Color ID: " << color_id << endl;
    add_color(C, name);
  }

  colorfs.close();
}

void readTEAM(connection * C) {
  std::ifstream teamfs("team.txt");
  string line;
  if (!teamfs.is_open()) {
    cerr << "Failed to open team.txt." << endl;
  }

  while (getline(teamfs, line)) {
    // NAME, STATE_ID, COLOR_ID, WINS, LOSSES
    int team_id;
    string name;
    int state_id;
    int color_id;
    int wins;
    int losses;
    stringstream ss(line);
    ss >> team_id >> name >> state_id >> color_id >> wins >> losses;
    // cout << "Team Name: " << name << ", State ID: " << state_id << ", Color ID: " << color_id << ", Wins: " << wins << ", Losses: " << losses << endl;
    add_team(C, name, state_id, color_id, wins, losses);
  }

  teamfs.close();
}

void readPLAYER(connection * C) {
  std::ifstream playerfs("player.txt");
  string line;
  if (!playerfs.is_open()) {
    cerr << "Failed to open player.txt." << endl;
  }

  while (getline(playerfs, line)) {
    // TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG
    int player_id;
    int team_id;
    int uniform_num;
    string first_name;
    string last_name;
    int mpg;
    int ppg;
    int rpg;
    int apg;
    double spg;
    double bpg;
    stringstream ss(line);
    ss >> player_id >> team_id >> uniform_num >> first_name >> last_name >> mpg >> ppg >> rpg >> apg >> spg >> bpg;
    // cout << "Player Team ID: " << team_id << ", Uniform_NUM: " << uniform_num << ", FIRST_NAME: " << first_name << ", LAST_NAME: " << last_name
    // << ", MPG: " << mpg << ", PPG: " << ppg << ", RPG: " << rpg << ", APG: " << apg << ", SPG: " << spg << ", BPG: " << bpg << endl;
    add_player(C, team_id, uniform_num, first_name, last_name, mpg, ppg, rpg, apg, spg, bpg);
  }

  playerfs.close();
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
  readSTATE(C);
  readCOLOR(C);
  readTEAM(C);
  readPLAYER(C);

  exercise(C);


  //Close database connection
  C->disconnect();

  return 0;
}


