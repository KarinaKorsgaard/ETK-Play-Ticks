//
//  ButtonData.h
//  _ETK_March_2017
//
//  Created by Karina Jensen on 07/03/17.
//
//

#ifndef ButtonData_h
#define ButtonData_h

struct ButtonData {
  public:
    int buttonID;
    int teamID;
    bool bHit;
    bool isPlaying;
    float contactStart;
};
struct RacketData {
  public:
    int teamID;
};

#endif /* ButtonData_h */
