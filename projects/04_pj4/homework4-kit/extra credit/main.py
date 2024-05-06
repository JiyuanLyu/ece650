from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from query import query1, query2, query3, query4, query5
from models import Base, Player, Team, State, Color

DATABASE_URL = "postgresql://postgres:passw0rd@localhost/ACC_BBALL"

engine = create_engine(DATABASE_URL)
Session = sessionmaker(bind=engine)

def main():
    session = Session()

    # Test cases for each query
    print("\nQuery 1: all attributes of each player with average statistics")
    query1(session, 1, 35, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

    print("\nQuery 2: name of each team with the indicated uniform color")
    query2(session, "DarkBlue")

    print("\nQuery 3: first and last name of each player that plays for the indicated team, ordered from highest to lowest ppg")
    query3(session, "Duke")

    print("\nQuery 4: uniform number, first name and last name of each player that plays in the indicated state and wears the indicated uniform color")
    query4(session, "NC", "DarkBlue")

    print("\nQuery 5: first name and last name of each player, and team name and number of wins for each team that has won more than the indicated number of games")
    query5(session, 12)

    session.close()

if __name__ == "__main__":
    main()
