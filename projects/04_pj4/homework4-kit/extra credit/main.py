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
    print("\nQuery 1: Players with Average Statistics in a Range")
    query1(session, 1, 35, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

    print("\nQuery 2: Teams with a Specific Uniform Color")
    query2(session, "DarkBlue")

    print("\nQuery 3: Players in a Team Ordered by PPG")
    query3(session, "Duke")

    print("\nQuery 4: Players in a State with a Specific Uniform Color")
    query4(session, "NC", "DarkBlue")

    print("\nQuery 5: Players and Teams with Wins Above a Threshold")
    query5(session, 10)

    session.close()

if __name__ == "__main__":
    main()
