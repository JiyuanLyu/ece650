#include "query_funcs.h"


void add_player(connection *C, int team_id, int jersey_num, string first_name, string last_name,
                int mpg, int ppg, int rpg, int apg, double spg, double bpg)
{
    work W(*C);
    stringstream player;
    player << "INSERT INTO PLAYER (TEAM_ID, UNIFORM_NUM, FIRST_NAME, LAST_NAME, MPG, PPG, RPG, APG, SPG, BPG) VALUES ("
    << team_id << ", " << jersey_num << ", " << W.quote(first_name) << ", " << W.quote(last_name) << ", " 
    << mpg << ", " << ppg << ", " << rpg << ", " << apg << ", " << spg << ", " << bpg << ");";
    W.exec(player.str());
    W.commit();
}


void add_team(connection *C, string name, int state_id, int color_id, int wins, int losses)
{
    work W(*C);
    stringstream team;
    team << "INSERT INTO TEAM (NAME, STATE_ID, COLOR_ID, WINS, LOSSES) VALUES ("
    << W.quote(name) << ", " << state_id << ", " << color_id << ", " << wins << ", " << losses << ");";
    W.exec(team.str());
    W.commit();
}


void add_state(connection *C, string name)
{
    work W(*C);
    stringstream state;
    state << "INSERT INTO STATE (NAME) VALUES (" << W.quote(name) << ");";
    W.exec(state.str());
    W.commit();
}


void add_color(connection *C, string name)
{
    work W(*C);
    stringstream color;
    color << "INSERT INTO COLOR (NAME) VALUES (" << W.quote(name) << ");";
    W.exec(color.str());
    W.commit();
}

/*
 * All use_ params are used as flags for corresponding attributes
 * a 1 for a use_ param means this attribute is enabled (i.e. a WHERE clause is needed)
 * a 0 for a use_ param means this attribute is disabled
 */
// query1(): show all attributes of each player with average statistics that fall between the min and max (inclusive) for each enabled statistic
void query1(connection *C,
	    int use_mpg, int min_mpg, int max_mpg,
            int use_ppg, int min_ppg, int max_ppg,
            int use_rpg, int min_rpg, int max_rpg,
            int use_apg, int min_apg, int max_apg,
            int use_spg, double min_spg, double max_spg,
            int use_bpg, double min_bpg, double max_bpg
            )
{
    int use[6] = {use_mpg, use_ppg, use_rpg, use_apg, use_spg, use_bpg};
    string column_names[6] = {"MPG", "PPG", "RPG", "APG", "SPG", "BPG"};

    int int_min[4] = {min_mpg, min_ppg, min_rpg, min_apg};
    int int_max[4] = {max_mpg, max_ppg, max_rpg, max_apg};
    double double_min[2] = {min_spg, min_bpg};
    double double_max[2] = {max_spg, max_bpg};

    stringstream query;
    query << "SELECT * FROM PLAYER";

    int conditionExist = 0;
    for (int i = 0; i < 6; i++) {
        if (use[i] != 0) {
            // add conditions query
            if (conditionExist == 0) {
                query << " WHERE ";
                conditionExist++;
            }
            else {
                query << " AND ";
            }

            // use int
            if (i < 4) {
                query << column_names[i] << " BETWEEN " << int_min[i] << " AND " << int_max[i];
            }
            // use double
            else {
                query << column_names[i] << " BETWEEN " << double_min[i-4] << " AND " << double_max[i-4];
            }
        }
    }
    query << ";";

    nontransaction N(*C);
    result R(N.exec(query.str()));
    cout << "PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<int>() << " " << c[1].as<int>() << " " << c[2].as<int>() << " " << c[3].as<string>() << " "
        << c[4].as<string>() << " " << c[5].as<int>() << " " << c[6].as<int>() << " " << c[7].as<int>() << " "
        << c[8].as<int>() << " " << fixed << setprecision(1) << c[9].as<double>() << " " << fixed << setprecision(1) << c[10].as<double>() << endl;
    }
}

// query2(): show the name of each team with the indicated uniform color
void query2(connection *C, string team_color)
{
    work W(*C);
    stringstream query;
    query << "SELECT TEAM.NAME FROM TEAM, COLOR "
    << "WHERE TEAM.COLOR_ID = COLOR.COLOR_ID "
    << "AND COLOR.NAME = " << W.quote(team_color) << ";";
    // W.commit();
    nontransaction N(*C);
    result R(N.exec(query.str()));
    cout << "NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << endl;
    }
}

// query3(): show the first and last name of each player that plays for the indicated team, ordered from highest to lowest ppg (points per game)
void query3(connection *C, string team_name)
{
    work W(*C);
    stringstream query;
    query << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
    << "FROM PLAYER, TEAM "
    << "WHERE TEAM.TEAM_ID = PLAYER.TEAM_ID "
    << "AND TEAM.NAME = " << W.quote(team_name) 
    << " ORDER BY PPG DESC;";
    // W.commit();
    nontransaction N(*C);
    result R(N.exec(query.str()));
    cout << "FIRST_NAME LAST_NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << endl;
    }
}

// // query4(): show uniform number, first name and last name of each player that plays in the indicated state and wears the indicated uniform color
// void query4(connection *C, string team_state, string team_color)
// {
//     work W(*C);
//     stringstream query;
//     query << "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
//     << "FROM PLAYER, TEAM, STATE, COLOR "
//     << "WHERE TEAM.TEAM_ID = PLAYER.TEAM_ID AND TEAM.STATE_ID = STATE.STATE_ID AND TEAM.COLOR_ID = COLOR.COLOR_ID "
//     << "AND STATE.NAME = " << W.quote(team_state)
//     << " AND COLOR.NAME = " << W.quote(team_color) << ";";
//     // W.commit();
//     nontransaction N(*C);
//     result R(N.exec(query.str()));
//     cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
//     for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
//         cout << c[0].as<int>() << c[1].as<string>() << c[2].as<string>() << endl;
//     }
// }

// query4(): show uniform number, first name and last name of each player that plays in the indicated state and wears the indicated uniform color
void query4(connection *C, string team_state, string team_color)
{
    work W(*C);
    stringstream query;
    query << "SELECT PLAYER.UNIFORM_NUM, PLAYER.FIRST_NAME, PLAYER.LAST_NAME "
    << "FROM PLAYER "
    << "JOIN TEAM ON PLAYER.TEAM_ID = TEAM.TEAM_ID "
    << "JOIN STATE ON TEAM.STATE_ID = STATE.STATE_ID "
    << "JOIN COLOR ON TEAM.COLOR_ID = COLOR.COLOR_ID "
    << "WHERE STATE.NAME = " << W.quote(team_state)
    << " AND COLOR.NAME = " << W.quote(team_color) << ";";
    // W.commit();
    nontransaction N(*C);
    result R(N.exec(query.str()));
    cout << "UNIFORM_NUM FIRST_NAME LAST_NAME" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<int>() << " " << c[1].as<string>() << " " << c[2].as<string>() << endl;
    }
}

// query5(): show first name and last name of each player, and team name and number of wins for each team that has won more than the indicated number of games
void query5(connection *C, int num_wins)
{
    work W(*C);
    stringstream query;
    query << "SELECT PLAYER.FIRST_NAME, PLAYER.LAST_NAME, TEAM.NAME, TEAM.WINS "
    << "FROM PLAYER, TEAM "
    << "WHERE TEAM.TEAM_ID = PLAYER.TEAM_ID "
    << "AND TEAM.WINS > " << num_wins << ";";
    // W.commit();
    nontransaction N(*C);
    result R(N.exec(query.str()));
    cout << "FIRST_NAME LAST_NAME NAME WINS" << endl;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        cout << c[0].as<string>() << " " << c[1].as<string>() << " " << c[2].as<string>() << " " << c[3].as<int>() << endl;
    }
}
