/**
 * @author Vitillo Roberto Agostino <vitillo@cli.di.unipi.it>
 */

#ifndef MESHSUBFILTER_H
#define MESHSUBFILTER_H

#include <common/meshmodel.h>
#include <common/interfaces.h>

class MeshSubFilter{
public:
    virtual void initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par) = 0;
    virtual bool applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb) = 0;

protected:
    static const float m_stepSize;
    static const unsigned int m_stepsPerSecond;
};

#endif // MESHSUBFILTER_H
