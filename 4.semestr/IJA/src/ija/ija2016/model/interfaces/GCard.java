/**
 * IJA - project
 * @file: GCard.java
 *
 * Authors:
 *         @author Filip Januš (xjanus08)
 *         @author Petr Jůda   (xjudap00)
 */
package ija2016.model.interfaces;

import java.awt.image.BufferedImage;

/**
 * Class represents interface for create graphic card after loading from file
 * @author Filip Januš (xjanus08)
 * @author Petr Jůda   (xjudap00)
 */
public interface GCard {
    /**
     * set image to card
     * @param valueImage front card image
     * @param BackImage back card image
     */
    void setImage(BufferedImage valueImage, BufferedImage BackImage);

}
