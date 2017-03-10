/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Game.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsidler <fsidler@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/12/15 16:25:43 by fsidler           #+#    #+#             */
/*   Updated: 2017/03/10 20:42:15 by fsidler          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Game.hpp"
#include <stdio.h>

Game::Game() : _main_win(NULL), _bottom_win(NULL), _timer(120), _score(0), _playerList(NULL), _enemyList(NULL), _bossList(NULL), _missilePlayerList(NULL), _missileEnemyList(NULL), _missileBossList(NULL), _missileBossList_missiles(NULL), _last_timer(0) {}

Game::Game(Game const &src) : _main_win(NULL), _bottom_win(NULL), _timer(src._timer), _score(src._score), _playerList(NULL), _enemyList(NULL)
{
    //fonction pour remplir toutes les listes (DEEP COPY!)
}

Game::~Game()
{
    _freeEntityList(_playerList);
    _freeEntityList(_enemyList);
    _freeEntityList(_bossList);
    _freeEntityList(_missilePlayerList);
    _freeEntityList(_missileEnemyList);
    _freeEntityList(_missileBossList);
    _freeEntityList(_missileBossList_missiles);
}

Game            &Game::operator=(Game const &rhs)
{
    if (this != &rhs)
    {
        _timer = rhs._timer;
        _score = rhs._score;
        wclear(_main_win);
        wclear(_bottom_win);
        delwin(_main_win);
        delwin(_bottom_win);
        _freeEntityList(_playerList);
        _freeEntityList(_enemyList);
        _freeEntityList(_bossList);
        _freeEntityList(_missilePlayerList);
        _freeEntityList(_missileEnemyList);
        _freeEntityList(_missileBossList);
        _freeEntityList(_missileBossList_missiles);
        //fonction pour remplir toutes les listes (DEEP COPY!)
    }
    return (*this);
}

void            Game::launch()
{
    _initGame();
    _gameLoop();
    _endGame();
}

void            Game::_initGame()
{
    t_coord playerCoord1;
    t_coord playerCoord2;

    initscr();
    if (LINES < MAIN_WIN_HMIN || COLS < MAIN_WIN_WMIN)
    {
        endwin();
        throw (Game::WindowDimensionsInvalidException());
    }
    _main_win = newwin(LINES - BOT_WIN_H, COLS, 0, 0);
    _bottom_win = newwin(BOT_WIN_H, COLS, LINES - BOT_WIN_H, 0);
	noecho();
	cbreak();
    curs_set(0);
	keypad(_main_win, TRUE);
	nodelay(_main_win, TRUE);
    set_escdelay(0);
    start_color();
    init_color(COLOR_BLUE, 600, 800, 1000);
    init_color(COLOR_WHITE, 1000, 700, 300);
    init_color(COLOR_BLACK, 0, 0, 0);
	init_color(COLOR_YELLOW, 220, 180, 120);
    init_color(COLOR_RED, 800, 0, 0);
    init_color(COLOR_MAGENTA, 800, 0, 800);
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    wbkgdset(_main_win, COLOR_PAIR(1));
    wbkgdset(_bottom_win, COLOR_PAIR(1));
    playerCoord1.y = LINES - (6 + BOT_WIN_H);
    playerCoord1.x = (COLS / 3) - 1;
    playerCoord2.y = LINES - (6 + BOT_WIN_H);
    playerCoord2.x = 2 * (COLS / 3) - 1;
    AWeapon     *missileboss = new Missileboss(2, 2, 50, "|", 500, 0);
    _pushInList(_playerList, new Player(3, 3, 2, _readSkin("env/playership.env"), missileboss->clone(), playerCoord1, 119, 115, 97, 100, KEY_SPC));
    _pushInList(_playerList, new Player(3, 3, 2, _readSkin("env/playership.env"), missileboss->clone(), playerCoord2, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, 48));//92
    delete missileboss;
}

void            Game::_gameLoop()
{
    int         i;
    int         key;
    bool        boss_pop = false;
    std::string bkgd;
    std::string game_over = _readSkin("env/gameover.env");
    std::string game_win = _readSkin("env/gamewin.env");
    AWeapon     *pioupiou = new Pioupiou(1, 1, 50, "|" , 1000);
    AWeapon     *missileboss = new Missileboss(2, 2, 50, _readSkin("env/stinger.env"), 2000, 0);

    bkgd = _fillBackground();
    while ((key = wgetch(_main_win)) != KEY_ESC && _playerList && _timer > 0 && (!boss_pop || _bossList))
    {
        _refreshMainWin(bkgd);
        i = rand();
        if (i % 5000 < 1 && _timer > 100)
            _pushInList(_enemyList, new Enemy(2, 3, 500, 250, _readSkin("env/enemy.env"), pioupiou->clone(), (t_coord){i % (COLS - 10) + 1, 1}));
        else if (!boss_pop && _timer <= 100)
        {
            _pushInList(_bossList, new Boss(50, 3, 100, 1000, _readSkin("env/shadow.env"), missileboss->clone(), (t_coord){30, 1}));
            delete missileboss;
            boss_pop = true;
        }
        _gameCore(key);
        wrefresh(_main_win);
        _refreshBottomWin(bkgd);
    }
    delete pioupiou;
    if (key != KEY_ESC)
    {
        werase(_main_win);
        mvwprintw(_main_win, 3, (COLS - 10) / 2, "FINAL SCORE:");
        mvwprintw(_main_win, 3, ((COLS - 3) / 2) + 14, "%i", _score);        
        if (!_playerList || _timer == 0)
            mvwprintw(_main_win, (LINES - BOT_WIN_H - 6) / 2, 1, game_over.c_str());
        else if (key != KEY_ESC)
            mvwprintw(_main_win, (LINES - BOT_WIN_H - 6) / 2, 1, game_win.c_str());
        wrefresh(_main_win);
        usleep(2000000);
    }
}

void            Game::_endGame()
{
    wclear(_main_win);
    wclear(_bottom_win);
    delwin(_main_win);
    delwin(_bottom_win);
    endwin();
}

void            Game::_gameCore(int key)
{
    _shootInList(_playerList, _missilePlayerList, key);
    _shootInList(_enemyList, _missileEnemyList, key);
    _shootInList(_bossList, _missileBossList, key);
    _shootInList(_missileBossList, _missileBossList_missiles, key);

    _displayEntities(_playerList, 3);
    _displayEntities(_enemyList, 1);
    _displayEntities(_bossList, 4);
    _displayEntities(_missilePlayerList, 3);
    _displayEntities(_missileEnemyList, 1);
    _displayEntities(_missileBossList, 5);
    _displayEntities(_missileBossList_missiles, 5);

    _moveInList(_playerList, key);
    _moveInList(_enemyList, key);
    _moveInList(_bossList, key);
    _moveInList(_missilePlayerList, key);
    _moveInList(_missileEnemyList, key);
    _moveInList(_missileBossList, key);
    _moveInList(_missileBossList_missiles, key);
    
    _collision(_playerList, _enemyList);
    _collision(_playerList, _bossList);
    _collision(_playerList, _missileEnemyList);
    _collision(_playerList, _missileBossList);
    _collision(_playerList, _missileBossList_missiles);
    _collision(_missilePlayerList, _enemyList);
    _collision(_missilePlayerList, _bossList);
	_collision(_missilePlayerList, _missileBossList);
}

void            Game::_displayEntities(AEntity::t_entityList *list, unsigned int color_nb) const
{
    AEntity::t_entityList    *list_tmp = list;

    while (list_tmp)
    {
        list_tmp->entity->displaySkin(_main_win, color_nb);
        list_tmp = list_tmp->next;
    }
}

void            Game::_moveInList(AEntity::t_entityList *&begin, int key)
{
    AEntity::t_entityList    *entity_tmp = begin;

    while (entity_tmp)
    {
        if (entity_tmp == begin && entity_tmp->entity->move(LINES - BOT_WIN_H, COLS, key) == false)
            _lstdelone(begin, entity_tmp, 'F');
        else if (entity_tmp->next && entity_tmp->next->entity->move(LINES - BOT_WIN_H, COLS, key) == false)
            _lstdelone(begin, entity_tmp, 'N');
        entity_tmp = entity_tmp ? entity_tmp->next : begin;
    }
}

void            Game::_shootInList(AEntity::t_entityList *list, AEntity::t_entityList *&listOfMissile, int key)
{
    while (list)
    {
        try { _pushInList(listOfMissile, list->entity->shoot(key)); }
        catch (std::exception &e) {}
        list = list->next;
    }
}

bool            Game::_hitbox(AEntity::t_entityList *entity1, AEntity::t_entityList *entity2) const
{
    if (entity1->entity->getCoord().x + entity1->entity->_skin_size.x >= entity2->entity->getCoord().x
        && entity1->entity->getCoord().x <= entity2->entity->getCoord().x + entity2->entity->_skin_size.x
        && entity1->entity->getCoord().y + entity1->entity->_skin_size.y - 1 >= entity2->entity->getCoord().y
        && entity1->entity->getCoord().y <= entity2->entity->getCoord().y + entity2->entity->_skin_size.y - 1)
        return (true);
    return (false);
}

void            Game::_collision(AEntity::t_entityList *&list1, AEntity::t_entityList *&list2)
{
    AEntity::t_entityList    *tmp1 = list1;
    unsigned int    tmp1HP;
    unsigned int    tmp2HP;

    while (tmp1)
    {
        AEntity::t_entityList    *tmp2 = list2;
        while (tmp1 && tmp2)
        {
            if (tmp1->next && tmp2->next && _hitbox(tmp1->next, tmp2->next))
            {
                tmp1HP = tmp1->next->entity->takeDamage(*tmp2->next->entity, _main_win);
                tmp2HP = tmp2->next->entity->takeDamage(*tmp1->next->entity, _main_win);
                if (!tmp1HP)
                {
                    _score += tmp1->entity->getScore();
                    _lstdelone(list1, tmp1, 'N');
                }
                if (!tmp2HP)
                {
                    _score += tmp2->entity->getScore();
                    _lstdelone(list2, tmp2, 'N');
                }
            }
            else if (tmp1->next && _hitbox(tmp1->next, tmp2))
            {
                tmp1HP = tmp1->next->entity->takeDamage(*tmp2->entity, _main_win);
                tmp2HP = tmp2->entity->takeDamage(*tmp1->next->entity, _main_win);
                if (!tmp1HP)
                {
                    _score += tmp1->entity->getScore();
                    _lstdelone(list1, tmp1, 'N');
                }
                if (!tmp2HP)
                {
                    _score += tmp2->entity->getScore();
                    _lstdelone(list2, tmp2, 'F');
                }
            }
            else if (tmp2->next && _hitbox(tmp1, tmp2->next))
            {
                tmp1HP = tmp1->entity->takeDamage(*tmp2->next->entity, _main_win);
                tmp2HP = tmp2->next->entity->takeDamage(*tmp1->entity, _main_win);
                if (!tmp1HP)
                {
                    _score += tmp1->entity->getScore();
                    _lstdelone(list1, tmp1, 'F');
                }
                if (!tmp2HP)
                {
                    _score += tmp2->entity->getScore();
                    _lstdelone(list2, tmp2, 'N');
                }
            }
            else if (_hitbox(tmp1, tmp2))
            {
                tmp1HP = tmp1->entity->takeDamage(*tmp2->entity, _main_win);
                tmp2HP = tmp2->entity->takeDamage(*tmp1->entity, _main_win);
                if (!tmp1HP)
                {
                    _score += tmp1->entity->getScore();
                    _lstdelone(list1, tmp1, 'F');
                }
                if (!tmp2HP)
                {
                    _score += tmp2->entity->getScore();
                    _lstdelone(list2, tmp2, 'F');
                }
            }
            tmp2 = tmp2 ? tmp2->next : list2;
        }
        tmp1 = tmp1 ? tmp1->next : list1;
    }
}

bool            Game::_checkTime(unsigned int msecond, clock_t  &last)
{
    clock_t        now;

    if (((now = clock()) * 1000 - last * 1000) / CLOCKS_PER_SEC >= msecond)
    {
        last = now;
        return (true);
    }
    return (false);
}

void            Game::_refreshMainWin(std::string bkgd) const
{
    wattron(_main_win, COLOR_PAIR(2));
    mvwprintw(_main_win, 1, 1, bkgd.c_str());
    wattroff(_main_win, COLOR_PAIR(2));
    box(_main_win, 0, 0);
}

void            Game::_refreshBottomWin(std::string bkgd)
{
    wattron(_bottom_win, COLOR_PAIR(2));
    mvwprintw(_bottom_win, 1, 1, bkgd.c_str());
    wattroff(_bottom_win, COLOR_PAIR(2));
    box(_bottom_win, 0, 0);
    mvwprintw(_bottom_win, 2, 3, "time:");
    mvwprintw(_bottom_win, 2, 19, "lives:");
    mvwprintw(_bottom_win, 2, 42, "score:");
    AEntity::t_entityList    *playerListTmp = _playerList;
    for (int y = 0; playerListTmp; y++)
    {
        int x = 26;
        for (unsigned int i = 0 ; i < playerListTmp->entity->getHp() ; i++)
        {
            mvwprintw(_bottom_win, 2 + y, x, "<3");
            x += 4;
        }
        playerListTmp = playerListTmp->next;
    }
    mvwprintw(_bottom_win, 2, 9, "%i", _timer);
    mvwprintw(_bottom_win, 2, 50, "%i", _score);    
    mvwvline(_bottom_win, 1, 16, ACS_VLINE, 3);
    mvwvline(_bottom_win, 1, 39, ACS_VLINE, 3);
    mvwvline(_bottom_win, 1, 60, ACS_VLINE, 3); 
    if (_checkTime(1000, _last_timer))
        _timer--;
    wrefresh(_bottom_win);
    werase(_bottom_win);
}

std::string     Game::_fillBackground() const
{
    int                 i;
    unsigned int        size = (LINES - BOT_WIN_H) * COLS;
    std::stringstream   bkgd;
    
    while (size--)
    {
        i = rand() % 100;
        if (i < 2)
            bkgd << '+';
        else if (i > 98)
            bkgd << '.';
        else
            bkgd << ' ';
    }
    return (bkgd.str());
}

std::string     Game::_readSkin(std::string nameOfFile) const
{
    std::ifstream       file(nameOfFile);
    std::stringstream   read;

    read << file.rdbuf();
    file.close();
    return (read.str());
}

//copy creation and operator= must send a clone of entity to Game::_pushInList (cf. d04/ex02/Squad.cpp)

void            Game::_pushInList(AEntity::t_entityList *&list, AEntity *entity)
{
    if (entity)
    {
        AEntity::t_entityList    *tmp = list;
        AEntity::t_entityList    *newEntity = new AEntity::t_entityList();

        newEntity->entity = entity;
        newEntity->next = NULL;
        while (tmp && tmp->next)
            tmp = tmp->next;
        tmp ? tmp->next = newEntity : list = newEntity;
    }
}

void            Game::_pushInList(AEntity::t_entityList *&dest, AEntity::t_entityList *src)
{
    if (src)
    {
        AEntity::t_entityList *tmp = dest;
        while (tmp && tmp->next)
            tmp = tmp->next;
        tmp ? tmp->next = src : dest = src;
    }
}

void            Game::_lstdelone(AEntity::t_entityList *&begin, AEntity::t_entityList *&current, char command)
{
    AEntity::t_entityList    *tmp_next = NULL;

    if (current == begin && command == 'F')
    {
        begin = current->next;
        delete current->entity;
        delete current;
        current = NULL;
    }
    else if (current->next)
    {
        tmp_next = current->next->next;
        delete current->next->entity;
        delete current->next;
        current->next = tmp_next;
    }
}

void            Game::_freeEntityList(AEntity::t_entityList *&list)
{
    AEntity::t_entityList    *tmp;

    while (list)
    {
        tmp = list->next;
        delete list->entity;
        delete list;
        list = tmp;
    }
    list = NULL;
}

Game::WindowDimensionsInvalidException::WindowDimensionsInvalidException() {}

Game::WindowDimensionsInvalidException::WindowDimensionsInvalidException(WindowDimensionsInvalidException const &src) { *this = src; }

Game::WindowDimensionsInvalidException::~WindowDimensionsInvalidException() throw() {}

Game::WindowDimensionsInvalidException  &Game::WindowDimensionsInvalidException::operator=(Game::WindowDimensionsInvalidException const &rhs)
{
    (void)rhs;
    return (*this);
}

char const                              *Game::WindowDimensionsInvalidException::what(void) const throw() { return ("Window dimensions are invalid"); }
