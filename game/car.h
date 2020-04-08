#ifndef _CAR_H_
#define _CAR_H_

class Car{	
	
	float m_speed;	
public:
	int  m_x;
	int  m_y;
	char m_avatar;

	Car(){
		this->m_avatar = ' ';
	}
	void SetPosition(int _x, int _y){
		this->m_x = _x;
		this->m_y = _y;
	}

	void SetAvatar(char _av){
		this->m_avatar = _av;
	}

	bool Move(char _move, unsigned _r, unsigned _c){
		switch(_move){
			case 'j':
			case 'J':{
				m_y -= 1;
				if(m_y < 2)
					return false;				
				break;
			}
			case 'l':
			case 'L':{
				m_y += 1;
				if(m_y > (_c-3))
					return false;
				break;
			}
			default:
				break;
		}

		return true;
	}

};

#endif