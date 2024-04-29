from sqlalchemy import create_engine, Column, Integer, String, ForeignKey, DECIMAL
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import relationship, sessionmaker

Base = declarative_base()

class State(Base):
    __tablename__ = 'state'
    state_id = Column(Integer, primary_key=True)
    name = Column(String(255), nullable=False)

class Color(Base):
    __tablename__ = 'color'
    color_id = Column(Integer, primary_key=True)
    name = Column(String(255), nullable=False)

class Team(Base):
    __tablename__ = 'team'
    team_id = Column(Integer, primary_key=True)
    name = Column(String(255), nullable=False)
    state_id = Column(Integer, ForeignKey('state.state_id', ondelete='SET NULL', onupdate='CASCADE'))
    color_id = Column(Integer, ForeignKey('color.color_id', ondelete='SET NULL', onupdate='CASCADE'))
    wins = Column(Integer)
    losses = Column(Integer)
    state = relationship("State", backref="teams")
    color = relationship("Color", backref="teams")

class Player(Base):
    __tablename__ = 'player'
    player_id = Column(Integer, primary_key=True)
    team_id = Column(Integer, ForeignKey('team.team_id', ondelete='SET NULL', onupdate='CASCADE'))
    uniform_num = Column(Integer)
    first_name = Column(String(255), nullable=False)
    last_name = Column(String(255), nullable=False)
    mpg = Column(Integer)
    ppg = Column(Integer)
    rpg = Column(Integer)
    apg = Column(Integer)
    spg = Column(DECIMAL(10,1))
    bpg = Column(DECIMAL(10,1))
    team = relationship("Team", backref="players")
