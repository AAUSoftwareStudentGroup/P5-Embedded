#!/usr/bin/python

import curses
from Mote import *

import signal
import sys, time
 
def signal_term_handler(signal, frame):
    print 'got SIGTERM'
    Mote.GracefullStop()
    sys.exit(0)
 
signal.signal(signal.SIGTERM, signal_term_handler)

def main(screen):
    menuIndex = 0
    menuScroll = 0
    running = True
    
    screen.nodelay(1)
    curses.curs_set(0)

    Mote.ReIndex(screen)
    Mote.StartMonitoring()
    menu = curses.newpad(300, 300)
    
    while(running):
        # Key options
        screen.addstr(curses.LINES-1,  0, ' R - Refresh ', curses.A_REVERSE)
        screen.addstr(curses.LINES-1, 16, ' Q - Quit ', curses.A_REVERSE)
        screen.addstr(curses.LINES-1, 29, ' ENTER - Configure Mote ', curses.A_REVERSE)
        screen.addstr(curses.LINES-1, 56, ' S - Start ', curses.A_REVERSE)

        for i in range(len(Mote.motes)):
            m = Mote.motes[i]
            if m.diffTime == -1:
                continue 

            menu.addstr(i,1,'*' if m.file != None else ' ')
            highlight = curses.A_NORMAL
            if menuIndex == i:
                highlight = curses.A_REVERSE
            name = m.file if m.file != None else m.ip
            menu.addstr(i, 3, '{} - dt{}: tx{} - rx{} '.format(name, m.diffTime, str(round(float(m.tx)/(2<<15),2)).rjust(5), str(round(float(m.rx)/(2<<15),2)).rjust(5)).ljust(curses.COLS-4), highlight)
        
        if menuIndex > menuScroll+curses.LINES-3:
            menuScroll += 1
        if menuIndex < menuScroll:
            menuScroll -= 1

        screen.refresh()
        menu.refresh(menuScroll,0,0,0,curses.LINES-3,curses.COLS-1)
        
        redrawWait = int(round(time.time() * 1000))+1000/30
        while redrawWait > int(round(time.time() * 1000)):
            c = screen.getch()
            if c == ord('q'):
                running = False   
            elif c == ord('r'):
                Mote.ReIndex(screen)
            elif c == ord('s'):
                menu.clear()
                screen.clear()
                StartSampling(screen)
                screen.clear()
            elif c == curses.KEY_UP:
                menuIndex -= 0 if menuIndex == 0 else 1
            elif c == curses.KEY_DOWN:
                menuIndex += 0 if menuIndex == len(Mote.motes)-1 else 1
            elif c == curses.KEY_ENTER or c == 10 or c == 13:
                configureMote(screen, Mote.motes[menuIndex])
    
    Mote.GracefullStop()

###########################

def promptUser(screen, prompt, bottomPrompt):
    # panel = curses.newwin(height, width, y, x)
    panelHeight = 5
    panelWidth = 50
    panel = curses.newwin(panelHeight, panelWidth, int((curses.LINES-panelHeight)/2), int((curses.COLS-panelWidth)/2))

    panel.box()
    panel.addstr(0, int(panelHeight/2), prompt)
    panel.addstr(panelHeight-1, panelWidth-3-len(bottomPrompt), bottomPrompt)
    panel.refresh()
    
    
    curses.curs_set(1)
    curses.echo()            # Enable echoing of characters

    # Get a 15-character string, with the cursor on the top line
    result = panel.getstr(2,2, panelWidth-1)

    curses.curs_set(0)
    # screen.clear();

    return result


def StartSampling(screen):
    menuIndex = 0
    menuScroll = 0
    sampling = True
    Mote.StartSampling()

    menu = curses.newpad(300, 300)
    while(sampling):
        # Key options
        screen.addstr(curses.LINES-1, 0, ' ESC - Stop sampling ', curses.A_REVERSE)

        for i in range(len(Mote.samplingMotes)):
            m = Mote.motes[i]

            menu.addstr(i,1,'*' if m.file != None else ' ')
            highlight = curses.A_NORMAL
            if menuIndex == i:
                highlight = curses.A_REVERSE
            name = m.file if m.file != None else m.ip
            menu.addstr(i, 3, '{}: dt{} - tx{} - rx{} '.format(name, m.diffTime, str(round(float(m.tx)/(2<<15),2)).rjust(5), str(round(float(m.rx)/(2<<15),2)).rjust(5)).ljust(curses.COLS-4), highlight)
        
        if menuIndex > menuScroll+curses.LINES-3:
            menuScroll += 1
        if menuIndex < menuScroll:
            menuScroll -= 1

        screen.refresh()
        menu.refresh(menuScroll,0,0,0,curses.LINES-3,curses.COLS-1)
        
        redrawWait = int(round(time.time() * 1000))+1000/30
        while redrawWait > int(round(time.time() * 1000)):
            c = screen.getch()
            if c == 27: # ESC key
                sampling = False
                Mote.samplingMotes = None

def configureMoteFilename(screen, mote):
    filename = promptUser(screen, 'File Name', 'Empty input to disable')
    mote.file = filename if len(filename) > 0 else None
    return

def configureMoteLabels(screen, mote):
    return

def configureMote(screen, mote):
    menu = [
        ("Set filename", configureMoteFilename),
        ("Manage labels", configureMoteLabels),
        ("Done", None),
    ]

    panelHeight = 4+len(menu)
    panelWidth = 50
    panel = curses.newwin(panelHeight, panelWidth, int((curses.LINES-panelHeight)/2), int((curses.COLS-panelWidth)/2))

    menuIndex = 0
    MenuIsOpen = True
    while MenuIsOpen:
        panel.box()
        panel.addstr(0, int(panelHeight/2), "Select parameter to configure")
        panel.addstr(panelHeight-1, panelWidth-3-len(""), "")
        
        for i in range(len(menu)):
            label,_ = menu[i]
            highlight = curses.A_NORMAL if menuIndex != i else curses.A_REVERSE
            panel.addstr(2+i, 2, label, highlight)

        panel.refresh()

        redrawWait = int(round(time.time() * 1000))+1000/30
        while redrawWait > int(round(time.time() * 1000)):
            c = screen.getch()
            if c == curses.KEY_UP:
                menuIndex -= 0 if menuIndex == 0 else 1
            elif c == curses.KEY_DOWN:
                menuIndex += 0 if menuIndex == len(menu)-1 else 1
            elif c == curses.KEY_ENTER or c == 10 or c == 13:
                if menuIndex == len(menu)-1:
                    MenuIsOpen = False
                else:
                    panel.clear()
                    panel.refresh()
                    _,func = menu[menuIndex]
                    func(screen, mote)

curses.wrapper(main)
