#ifndef STEP_H
#define STEP_H

#include <ClickPoint.h>

class Step{
public:
    ClickPoint StartPoint;
    ClickPoint EndPoint;
    int start;
    int end;
public:
    Step(ClickPoint s,ClickPoint e, int ss,int ee)
        :StartPoint(s),EndPoint(e),start(ss),end(ee){}
};

#endif // STEP_H
