/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Game.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsidler <fsidler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/15 16:14:47 by fsidler           #+#    #+#             */
/*   Updated: 2017/03/13 21:41:43 by fsidler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GAME_HPP
# define GAME_HPP

# include "AEntity.hpp"
# include "Weapon.hpp"
# include "Missile.hpp"
# include "Bonus.hpp"
# include "Player.hpp"
# include "Enemy.hpp"
# include "Boss.hpp"
# include "WeapTwoMissSameSide.hpp"

# define MAIN_WIN_WMIN 130
# define MAIN_WIN_HMIN 30

# define KEY_SPC 32
# define KEY_ESC 27

# define COL_DEFAULT 1
# define COL_YELLOW 2
# define COL_BLUE 3
# define COL_RED 4
# define COL_PURPLE 5
# define COL_GREEN 6

//class                   Player;

class                   Game {

	public:
		Game();
		Game(Game const &src);
		~Game();

		Game            &operator=(Game const &rhs);
		void            launch();
		static void     pushInList(AEntity::t_entityList *&list, AEntity *entity);
		static void     pushInList(AEntity::t_entityList *&dest, AEntity::t_entityList *src);
		static bool     checkTime(unsigned int msecond, clock_t &last);
		
	private:
		class WindowDimensionsInvalidException : public std::exception {

			public:
				WindowDimensionsInvalidException();
				WindowDimensionsInvalidException(WindowDimensionsInvalidException const &src);
				virtual ~WindowDimensionsInvalidException() throw();

				WindowDimensionsInvalidException    &operator=(WindowDimensionsInvalidException const &rhs);
				
				char const                          *what(void) const throw();
				
		};

		void            _initGame();
		void            _gameLoop();
		void            _endGame();
		
		void            _gameCore(int key);
		void            _displayEntities(AEntity::t_entityList *list, unsigned int colorNb) const;
		void			_moveInList(AEntity::t_entityList *&begin, int key);
		void            _shootInList(AEntity::t_entityList *list, AEntity::t_entityList *&listOfMissile, int key);
		bool            _hitbox(AEntity::t_entityList *entity1, AEntity::t_entityList *entity2) const;
		void            _collision(AEntity::t_entityList *&list1, AEntity::t_entityList *&list2);

		void            _refreshMainWin(std::string bkgd) const;
		void            _refreshBottomWin(std::string bkgd);
		
		void            _lstdelone(AEntity::t_entityList *&begin, AEntity::t_entityList *&current, char command);
		void            _freeEntityList(AEntity::t_entityList *&list);
		void            _copyEntityList(AEntity::t_entityList *&dest, AEntity::t_entityList *src);

		std::string		_fillBackground() const;
		std::string		_readSkin(std::string nameOfFile) const;

		WINDOW          *_mainWin;
		WINDOW          *_bottomWin;
		
		unsigned int    _timer;
		unsigned int    _score;

		AEntity::t_entityList	*_playerList;
		AEntity::t_entityList	*_enemyList;
		AEntity::t_entityList	*_bossList;
		AEntity::t_entityList	*_missilePlayerList;
		AEntity::t_entityList	*_missileEnemyList;
		AEntity::t_entityList	*_missileBossList;
		AEntity::t_entityList	*_bonusList;

		clock_t         _lastTimer;

};

#endif