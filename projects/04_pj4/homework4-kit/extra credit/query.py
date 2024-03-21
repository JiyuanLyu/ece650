from sqlalchemy.orm import sessionmaker
from sqlalchemy import create_engine
from models import Base, Player, Team, State, Color

# query1(): show all attributes of each player with average statistics that fall between the min and max (inclusive) for each enabled statistic
def query1(session, use_mpg, min_mpg, max_mpg, use_ppg, min_ppg, max_ppg, use_rpg, min_rpg, max_rpg, use_apg, min_apg, max_apg, use_spg, min_spg, max_spg, use_bpg, min_bpg, max_bpg):
    print("PLAYER_ID TEAM_ID UNIFORM_NUM FIRST_NAME LAST_NAME MPG PPG RPG APG SPG BPG")
    query = session.query(Player)
    if use_mpg:
        query = query.filter(Player.mpg.between(min_mpg, max_mpg))
    if use_ppg:
        query = query.filter(Player.ppg.between(min_ppg, max_ppg))
    if use_rpg:
        query = query.filter(Player.rpg.between(min_rpg, max_rpg))
    if use_apg:
        query = query.filter(Player.apg.between(min_apg, max_apg))
    if use_spg:
        query = query.filter(Player.spg.between(min_spg, max_spg))
    if use_bpg:
        query = query.filter(Player.bpg.between(min_bpg, max_bpg))
    
    for player in query.all():
        print(player.player_id, player.team_id, player.uniform_num, player.first_name, player.last_name, player.mpg, player.ppg, player.rpg, player.apg, format(player.spg, '.1f'), format(player.bpg, '.1f'))

# query2(): show the name of each team with the indicated uniform color
def query2(session, team_color):
    print("NAME")
    query = session.query(Team.name).join(Color).filter(Color.name == team_color)
    for team_name, in query.all():
        print(team_name)

# query3(): show the first and last name of each player that plays for the indicated team, ordered from highest to lowest ppg (points per game)
def query3(session, team_name):
    print("FIRST_NAME LAST_NAME")
    query = session.query(Player.first_name, Player.last_name).join(Team).filter(Team.name == team_name).order_by(Player.ppg.desc())
    for first_name, last_name in query.all():
        print(first_name, last_name)


# query4(): show uniform number, first name and last name of each player that plays in the indicated state and wears the indicated uniform color
def query4(session, team_state, team_color):
    print("UNIFORM_NUM FIRST_NAME LAST_NAME")
    query = session.query(Player.uniform_num, Player.first_name, Player.last_name).join(Team).join(State).join(Color).filter(State.name == team_state, Color.name == team_color)
    for uniform_num, first_name, last_name in query.all():
        print(uniform_num, first_name, last_name)


# query5(): show first name and last name of each player, and team name and number of wins for each team that has won more than the indicated number of games
def query5(session, num_wins):
    print("FIRST_NAME LAST_NAME NAME WINS")
    query = session.query(Player.first_name, Player.last_name, Team.name, Team.wins).join(Team).filter(Team.wins > num_wins)
    for first_name, last_name, team_name, wins in query.all():
        print(first_name, last_name, team_name, wins)
