/**
 *
 * Project: Solitaire Klondike
 * Authors: Vladimír Jeřábek (xjerab21)
 *          Petr Jůda  (xjudap00)
 *
 * Class representing card in GUI (contains pixmap, and clicabel event)
 *
 */

#ifndef CARDGI_H
#define CARDGI_H

#include <QLabel>
#include "card.hpp"


class CardGI : public QLabel
{
    Q_OBJECT

public:
    /**
     * Explicit constructor of Card Graphic Interface (cardGI)
     * @param text
     * @param parent
     * @param card
     * @return CardGI *
     */
    explicit CardGI( const QString & text="", QWidget * parent = 0, KCard * card = nullptr) ;
    /**
     * Method, which render card, whith info from card
     * if card is not equal with card -> render again
     * @param card
     */
    void rewrite(KCard * card);
    //!Name of stack, where is card lockated
    QString CardStack = "";
    //!Pointer to card in decks
    KCard * card_loc = nullptr;
    /**
     * Methods for get Full name of Card -> it is used for STATUS
     * @return full name of card
     */
    QString getCardName();
    ~CardGI();


signals:
    /**
     * signal, when card is clicked
     * @param send -> card, which was clicked
     */
    void clicked(CardGI * send);

protected:
    /**
     * Method which emit signal clicked();
     * @param event
     */
    void mousePressEvent( QMouseEvent * event );
};

#endif // CARDGI_H
