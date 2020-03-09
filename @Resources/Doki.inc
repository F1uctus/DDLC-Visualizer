[Variables]
    ; Movement constants
    rightx=40
    edgey=50
    middley=20
    jumpdelay=140

    ; Update divisors for movement sources
    rnd_update_rate=200
    mus_update_rate=20

[MeasureAudio]
    Measure=Plugin
    Plugin=AudioLevel
    Port=Output
    FFTSize=1024
    FFTAttack=100
    FFTDecay=100
    Bands=5
    UpdateDivider=#mus_update_rate#

[MeasureBand0]
    Measure=Plugin
    Plugin=AudioLevel
    Parent=MeasureAudio
    Type=Band
    BandIdx=0
    UpdateDivider=#mus_update_rate#
    Disabled=1
    Group=audio

[MeasureBand1]
    Measure=Plugin
    Plugin=AudioLevel
    Parent=MeasureAudio
    Type=Band
    BandIdx=1
    UpdateDivider=#mus_update_rate#
    Disabled=1
    Group=audio

[MeasureBand2]
    Measure=Plugin
    Plugin=AudioLevel
    Parent=MeasureAudio
    Type=Band
    BandIdx=2
    UpdateDivider=#mus_update_rate#
    Disabled=1
    Group=audio

[MeasureBand3]
    Measure=Plugin
    Plugin=AudioLevel
    Parent=MeasureAudio
    Type=Band
    BandIdx=3
    UpdateDivider=#mus_update_rate#
    Disabled=1
    Group=audio

[MeasureBand4]
    Measure=Plugin
    Plugin=AudioLevel
    Parent=MeasureAudio
    Type=Band
    BandIdx=4
    UpdateDivider=#mus_update_rate#
    Disabled=1
    Group=audio

; For each doki's movement we have a band measure
; then, we select strongest band at the moment
; through the lua script, and set it's index as movement.
[MeasureMusicMovementSource]
    Measure=Script
    ScriptFile=#@#IdxOfMax.lua
    UpdateDivider=#mus_update_rate#
    Disabled=1
    Group=audio

[MeasureRandomMovementSource]
    Measure=Calc
    Formula=Random
    UpdateRandom=1
    UniqueRandom=1
    HighBound=4
    UpdateDivider=#rnd_update_rate#

; This selects what source of movements to use:
; If music is not playing at the moment, we switch
; to slow random movements.
; Else, we use strongest band index as a movement
; and update more frequently.
[MeasureMovementsSource]
    Measure=Calc
    IfCondition=MeasureAudio < 0.005
    IfTrueAction=[!DisableMeasureGroup audio][!SetOption MeasureState Formula [*MeasureRandomMovementSource*]][!SetOption MeasureState UpdateDivider #rnd_update_rate#]
    IfFalseAction=[!EnableMeasureGroup audio][!SetOption MeasureState Formula [*MeasureMusicMovementSource*]][!SetOption MeasureState UpdateDivider #mus_update_rate#]
    UpdateDivider=#mus_update_rate#

; Doki's finite state machine :D
[MeasureState]
    Measure=Calc
    Formula=MeasureRandomMovementSource
    ; none (0) is empty state, nothing to do.
    ; 1: jump
    IfCondition=MeasureState = 1
    IfTrueAction=[!SetOption MeasureY InvertMeasure 0]
    ; 2: flip left
    IfCondition2=MeasureState = 2
    IfTrueAction2=[!SetOption Doki ImageFlip Horizontal][!SetOption MeasureY InvertMeasure 0][!UnpauseMeasure MeasureY]
    ; 3: flip right
    IfCondition3=MeasureState = 3
    IfTrueAction3=[!SetOption Doki ImageFlip None][!SetOption MeasureY InvertMeasure 0][!UnpauseMeasure MeasureY]
    ; 4: side jump
    ; forward
    IfCondition4=(MeasureState = 4) && (MeasureX = 0)
    IfTrueAction4=[!SetOption Doki ImageFlip None][!SetOption MeasureY InvertMeasure 0][!UnpauseMeasure MeasureY][!SetOption MeasureX InvertMeasure 0][!UnpauseMeasure MeasureX]
    ; backward
    IfCondition5=(MeasureState = 4) && (MeasureX = #rightx#)
    IfTrueAction5=[!SetOption Doki ImageFlip Horizontal][!SetOption MeasureY InvertMeasure 0][!UnpauseMeasure MeasureY][!SetOption MeasureX InvertMeasure 1][!UnpauseMeasure MeasureX]
    ; 5: yay
    IfCondition6=MeasureState = 5
    IfTrueAction6=[!CommandMeasure MeasureYayMovement "Execute 2"]

    UpdateDivider=#rnd_update_rate#
    DynamicVariables=1

[MeasureY]
    Measure=Loop
    StartValue=#edgey#
    EndValue=#middley#
    Increment=-6
    LoopCount=1
    Paused=1

; This moves doki down in middle of a jump.
[MeasureYMiddleDown]
    Measure=Calc
    IfCondition=MeasureY = #middley#
    IfTrueAction=[!SetOption MeasureY InvertMeasure 1]

[MeasureX]
    Measure=Loop
    StartValue=0
    EndValue=#rightx#
    Increment=3
    LoopCount=1
    Paused=1

; This animates a 'yay' jump
[MeasureYay]
    Measure=Loop
    StartValue=#edgey#
    EndValue=0
    Increment=-6
    LoopCount=1
    Disabled=1
    OnChangeAction=[!SetOption Doki Y [MeasureYay]]

[MeasureYayMovement]
    Measure=Plugin
    Plugin=ActionTimer
    ; On click: Jump twice
    ActionList1=Start | Jump1Up | Wait #jumpdelay# | JumpDown | Wait #jumpdelay# | Jump2Up | Wait #jumpdelay# | JumpDown | Wait #jumpdelay# | End
    ; On beat: Jump once
    ActionList2=Start | Jump1Up | Wait #jumpdelay# | JumpDown | Wait #jumpdelay# | End
    ; Pausing 'MeasureState' prevents
    ; messing with doki state, because jump
    ; is executed in another thread, but beat
    ; processing still works and can change the state.
    Start=[!SetOption Doki ImageName "#@#Images\DokisJump\#doki_name#.png"][!PauseMeasure MeasureState]
    ; High jump
    Jump1Up=[!SetOption MeasureYay EndValue 0][!SetOption MeasureYay InvertMeasure 0][!EnableMeasure MeasureYay]
    ; Short jump
    Jump2Up=[!SetOption MeasureYay EndValue 20][!SetOption MeasureYay InvertMeasure 0][!EnableMeasure MeasureYay]
    JumpDown=[!SetOption MeasureYay InvertMeasure 1]
    ; Resume 'MeasureState'
    End=[!DisableMeasure MeasureYay][!SetOption Doki ImageName "#@#Images\Dokis\#doki_name#.png"][!SetOption Doki Y [*MeasureY*]][!UnpauseMeasure MeasureState]

[BGStyle]
    ; This fixes 0-transparency glitches on some graphic cards
    Shape=Rectangle 0,0,(#CURRENTCONFIGWIDTH#*1.5),#CURRENTCONFIGHEIGHT# | Fill Color 0,0,0,1 | StrokeWidth 0 
    DynamicVariables=1
    UpdateDivider=-1

[DokiStyle]
    ImageName=#@#Images\Dokis\#doki_name#.png
    X=[MeasureX]
    Y=[MeasureY]
    DynamicVariables=1
    LeftMouseDownAction=[!CommandMeasure MeasureYayMovement "Execute 1"]