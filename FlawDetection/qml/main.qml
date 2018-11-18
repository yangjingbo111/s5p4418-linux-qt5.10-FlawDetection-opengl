import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import AppManager 1.0
import Ft2232HWrapper 1.0
import Ascan 1.0
import AscanFbo 1.0


import "../js/Utils.js" as Utils
Window {
    id: app
    visible: true
    width: 640
    height: 480
    color: "black"
    title: qsTr("Flaw Detection")

    // date and time
    property date currentTime: new Date()
    property string timeStr: Qt.formatDateTime(currentTime, "yyyy-MM-dd hh:mm:ss")

    // battery and charge
    property int chg_donen: 0   // 0 charge done; 1 charging...
    property int pwr_src: 0     // 0 DC adapter;  1 Li battery
    property int bat_tmp_stage: 0   // battery tmp level [used for battery icon scroll]
    property int bat_cur_stage: 0   // battery current level
    property int bat_max_stage: 5


    property int focusItemIndex: 0
    // menu 1
    property int gainValue: 110
    property int rangeValue: 200
    property int highVolValue: 0
    property int emitDampValue: 0
    property int filterBandValue: 0
    // menu 2
    property int rectificationTypeValue: 0
    property int transducerTypeValue: 0
    property int tcgGainValue: 0
    property int keyBoardBacklightValue: 0
    property int vgaValue: 0
    //menu 3
    property int repeatFreqValue: 50
    property int hardwareDrawValue: 0
    property int echoDisplayModeValue: 0
    property int echoFreezeValue: 0
    property int batteryRecordValue: 0  //0 stopped 1 recording //test

    // battery
    property string info: ""
    property string adcval: "4000" // >3625
    property int recordcnt: 0


    //add value or sub value
    property string keyType: "" //left right up down


    AppManager{
        id: appManager

        onAdcReceived: {
            adcval = val
            var adcint = parseInt(adcval)
            if( adcint >= Utils.BAT_TAB[0]){        // 5 bars
                bat_cur_stage = 5
            }
            else if(adcint >= Utils.BAT_TAB[1]){    // 4 bars
                bat_cur_stage = 4
            }
            else if(adcint >= Utils.BAT_TAB[2]){    // 3 bars
                bat_cur_stage = 3
            }
            else if(adcint >= Utils.BAT_TAB[3]){
                bat_cur_stage = 2
            }
            else if(adcint >= Utils.BAT_TAB[4]){
                bat_cur_stage = 1
            }
            else if(adcint >= Utils.BAT_TAB[5]){
                bat_cur_stage = 0
                // enable low power tips

            }
            else if(adcint < Utils.BAT_TAB[6]){
                bat_cur_stage = -1 // very low power !!!

                // save params before poweroff

                // force poweroff
                appManager.powerOff()
            }
        }
        onPwrSrcChanged:{
            pwr_src = src
        }
        onChgStatusChanged:{
            chg_donen = chg
        }

    }

    // get battery adc value
    Timer{
        id: batterytimer
        interval: 30*1000  // 30s
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: {
            appManager.getadc(0)
        }
    }


    // battery record timer
    Timer{
        id: adctimer
        interval: 2*60*1000  //2min
        repeat: true
        running: false
        onTriggered: {
            appManager.getadc(0)
            status.text = "recording in every 2 min... "+recordcnt+": "+adcval
            recordcnt++
        }
    }

    Ft2232HWrapper{
        id: ft2232HWrapper

        // ft2232h data ready
        onDataReady: {

//            ascan.recvData(data)
            ascanfbo.recvData(data)
        }
    }


    // this timer triggered when button pressed for 1s
    Timer{
        id: long_press_timer
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            change_value_timer.restart()
            console.log("long_press_timer triggered")
        }
    }

    // refresh time every 500ms
    Timer{
        interval: 500
        repeat: true
        running: true

        onTriggered: {
            currentTime = new Date()
            timeStr = Qt.formatDateTime(currentTime, "yyyy-MM-dd hh:mm:ss")
        }
    }

    // manage battery charging icons scroll
    Timer{
        id: bat_charging_timer
        interval: 1000
        repeat: true
        running: false
        onTriggered: {
            bat_tmp_stage += 1
            if(bat_tmp_stage > bat_max_stage){
//                bat_tmp_stage = bat_cur_stage > -1 ? bat_cur_stage : 0  // range[current, current+1,... max]
                bat_tmp_stage = 0   // range[0-1-2-3-4-5-0...]
            }
            console.log("---------", bat_tmp_stage)
        }
    }

    // triggered to speed up value change of key press
    Timer{
        id: change_value_timer
        interval: 100
        running: false
        repeat: false
        onTriggered: {
            console.log("change_value_timer triggered")
            if(app.keyType === "right"){
                app.gainValue += 2

                ft2232HWrapper.wrGain(app.gainValue)
            }
            else if(app.keyType === "left"){
                if(app.gainValue > 2){
                    app.gainValue -= 2

                    ft2232HWrapper.wrGain(app.gainValue)
                }


            }

            change_value_timer.restart()
        }
    }


    Column{
        spacing: 0
        width: parent.width

        Rectangle{
            width: parent.width
            height: 60
            color: "gray"

            Text {
                id: battery
                anchors.centerIn: parent
                text: adcval
            }

            Item {
                id: battery_container
                anchors.right: time.left
                anchors.rightMargin: 8
                width: 30
                height: parent.height

                Image {
                    id: battery_icon
                    anchors.centerIn: parent
                    source: {
                        if(pwr_src === 1){  // Li battery
                            bat_charging_timer.running = false  // stop timer
                            return Utils.BAT_BARS[bat_cur_stage > -1 ? bat_max_stage - bat_cur_stage : 0]

                        }
                        else{   // DC adapter
                            if(chg_donen === 1){// charging...
                                bat_charging_timer.restart()

//                                bat_tmp_stage = bat_cur_stage   // initialize
                                return Utils.BAT_BARS[bat_tmp_stage > 5? 5:bat_max_stage - bat_tmp_stage]
                            }
                            else{   // charge done
                                bat_charging_timer.running = false
                                return Utils.BAT_CHG_DONE
                            }

                        }


                    }
                    width: 20
                    height: 40
                }
            }

            Text {
                id: time
                color: "white"
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: timeStr
            }

//            Button{
//                anchors.right: parent.right
//                anchors.verticalCenter: parent.verticalCenter
//                text: qsTr("exit")

//                onClicked: {
//                    ft2232HWrapper.closeFt2232H()
//                    appManager.startApp("/opt/Launcher/bin/Launcher")
//                }
//            }
        }


        Row {
            width: parent.width
            height: 400

//            Ascan {
//                id: ascan
//                width: 512
//                height: 400

//            }

            AscanFbo{
                id: ascanfbo
                width: 512
                height: 400
            }


            Item{
                id: menu_container
//                anchors.left: ascan.right
                anchors.left: ascanfbo.right
                focus: true
                visible: true



                Keys.onPressed: {

                    if(event.key === Utils.KEY_UP){   //KEY UP, SELECT THE UPSIDE ITEM
                        // control the hasFocus owner
                        if(app.focusItemIndex > 0) app.focusItemIndex -= 1
                        else if(app.focusItemIndex === 0)app.focusItemIndex = Utils.MENU_ITEM_NUM - 1

                    }
                    else if(event.key === Utils.KEY_DOWN){   //KEY DOWN, SELECT THE DOWNSIDE ITEM
                        if(app.focusItemIndex < Utils.MENU_ITEM_NUM - 1 /* 13 (14 include battery record) is now the MAX item num*/) app.focusItemIndex += 1
                        else if(app.focusItemIndex === Utils.MENU_ITEM_NUM - 1)app.focusItemIndex = 0
                    }
                    else if(event.key === Utils.KEY_RIGHT){   //KEY RIGHT +
                        app.keyType = "right"
                        if(gain.hasFocus){
                            long_press_timer.restart()    //

                            app.gainValue += 2

                            ft2232HWrapper.wrGain(app.gainValue)

                            ft2232HWrapper.testFun(0) // test key sound
                        }
                        else if(range.hasFocus){
                            app.rangeValue += 10

                            ft2232HWrapper.wrRange(app.rangeValue)
                        }
                        else if(high_vol.hasFocus){
                            if(app.highVolValue === 0)app.highVolValue = 1
                            else if(app.highVolValue === 1)app.highVolValue = 3
                            else if(app.highVolValue === 3)app.highVolValue = 7

                            ft2232HWrapper.wrHighVol(app.highVolValue)
                        }
                        else if(emit_damp.hasFocus){
                            if(app.emitDampValue === 0)app.emitDampValue = 1

                            ft2232HWrapper.wrEmitDamp(app.emitDampValue)
                        }
                        else if(filter_band.hasFocus){
                            if(app.filterBandValue === 0)app.filterBandValue = 2
                            else if(app.filterBandValue === 2)app.filterBandValue = 3

                            ft2232HWrapper.wrFilterBand(app.filterBandValue)
                        }
                        // menu 2
                        else if(rectificationType.hasFocus){
                            if(app.rectificationTypeValue < 3)app.rectificationTypeValue += 1

                            ft2232HWrapper.wrrectificationType(app.rectificationTypeValue)
//                            ascan.setrectificationType(app.rectificationTypeValue)
                            ascanfbo.setrectificationType(app.rectificationTypeValue)

                        }
                        else if(transducerType.hasFocus){
                            if(app.transducerTypeValue < 1)app.transducerTypeValue += 1

                            ft2232HWrapper.wrtransducerType(app.transducerTypeValue)
                        }
                        else if(tcgGain.hasFocus){
                            if(app.tcgGainValue < 1)app.tcgGainValue += 1

                            ft2232HWrapper.wrtcgGain(app.tcgGainValue)
                        }
                        else if(keyBoardBacklight.hasFocus){
                            if(app.keyBoardBacklightValue < 3)app.keyBoardBacklightValue += 1

                            ft2232HWrapper.wrkeyBoardBacklight(app.keyBoardBacklightValue)
                        }
                        else if(vga.hasFocus){
                            if(app.vgaValue < 1)app.vgaValue += 1

                            ft2232HWrapper.wrvga(app.vgaValue)
                        }
                        //menu 3
                        else if(repeatFreq.hasFocus){
//                            if(app.repeatFreqValue < 1000)app.repeatFreqValue += 10
                            if(app.repeatFreqValue < 1000)app.repeatFreqValue += 5
                            ft2232HWrapper.wrrepeatFreq(app.repeatFreqValue)
                        }
                        else if(hardwareDraw.hasFocus){//
                            if(app.hardwareDrawValue === 0){
                                app.hardwareDrawValue = 1
                                ft2232HWrapper.wrhardwareDraw(app.hardwareDrawValue)
//                                ascan.sethardwareDraw(app.hardwareDrawValue)
                                ascanfbo.sethardwareDraw(app.hardwareDrawValue)
                            }
                        }
                        else if(echoDisplayMode.hasFocus){
                            if(app.echoDisplayModeValue === 0){
                                app.echoDisplayModeValue = 1
                                ft2232HWrapper.wrechoDisplayMode(app.echoDisplayModeValue)
                            }
                        }
                        else if(echoFreeze.hasFocus){
                            if(app.echoFreezeValue === 0){
                                app.echoFreezeValue = 1
                                ft2232HWrapper.wrechoFreeze(app.echoFreezeValue)
                            }
                        }
                        else if(batteryRecord.hasFocus){
                            if(app.batteryRecordValue === 0){
                                app.batteryRecordValue = 1
                                status.text = "start recording in every 2 min... "
                                appManager.writeHeader("/opt/battery.txt")
                                adctimer.restart()
                                recordcnt = 0
//                                ft2232HWrapper.wrechoFreeze(app.echoFreezeValue)
                            }
                        }
                    }
                    else if(event.key === Utils.KEY_LEFT){  //KEY LEFT -
                        app.keyType = "left"
                        if(gain.hasFocus){
                            if(app.gainValue >= 2){
                                long_press_timer.restart()    //
                                app.gainValue -= 2

                                ft2232HWrapper.wrGain(app.gainValue)
                            }
                        }
                        else if(range.hasFocus){
                            if(app.rangeValue >= 10){
                                app.rangeValue -= 10

                                ft2232HWrapper.wrRange(app.rangeValue)
                            }
                        }
                        else if(high_vol.hasFocus){
                            if(app.highVolValue === 1)app.highVolValue = 0
                            else if(app.highVolValue === 3)app.highVolValue = 1
                            else if(app.highVolValue === 7)app.highVolValue = 3

                            ft2232HWrapper.wrHighVol(app.highVolValue)
                        }
                        else if(emit_damp.hasFocus){
                            if(app.emitDampValue === 1)app.emitDampValue = 0

                            ft2232HWrapper.wrEmitDamp(app.emitDampValue)
                        }
                        else if(filter_band.hasFocus){
                            if(app.filterBandValue === 3)app.filterBandValue = 2
                            else if(app.filterBandValue === 2)app.filterBandValue = 0

                            ft2232HWrapper.wrFilterBand(app.filterBandValue)
                        }
                        // menu 2
                        else if(rectificationType.hasFocus){
                            if(app.rectificationTypeValue > 0)app.rectificationTypeValue -= 1

                            ft2232HWrapper.wrrectificationType(app.rectificationTypeValue)
//                            ascan.setrectificationType(app.rectificationTypeValue)
                            ascanfbo.setrectificationType(app.rectificationTypeValue)
                        }
                        else if(transducerType.hasFocus){
                            if(app.transducerTypeValue > 0)app.transducerTypeValue -= 1

                            ft2232HWrapper.wrtransducerType(app.transducerTypeValue)
                        }
                        else if(tcgGain.hasFocus){
                            if(app.tcgGainValue > 0)app.tcgGainValue -= 1

                            ft2232HWrapper.wrtcgGain(app.tcgGainValue)
                        }
                        else if(keyBoardBacklight.hasFocus){
                            if(app.keyBoardBacklightValue > 0)app.keyBoardBacklightValue -= 1

                            ft2232HWrapper.wrkeyBoardBacklight(app.keyBoardBacklightValue)
                        }
                        else if(vga.hasFocus){
                            if(app.vgaValue > 0)app.vgaValue -= 1

                            ft2232HWrapper.wrvga(app.vgaValue)
                        }
                        //menu 3
                        else if(repeatFreq.hasFocus){
                            if(app.repeatFreqValue > 30)app.repeatFreqValue -= 5
//                            if(app.repeatFreqValue > 30)app.repeatFreqValue -= 2
                            ft2232HWrapper.wrrepeatFreq(app.repeatFreqValue)
                        }
                        else if(hardwareDraw.hasFocus){
                            if(app.hardwareDrawValue === 1){
                                app.hardwareDrawValue = 0
                                ft2232HWrapper.wrhardwareDraw(app.hardwareDrawValue)
//                                ascan.sethardwareDraw(app.hardwareDrawValue)
                                ascanfbo.sethardwareDraw(app.hardwareDrawValue)
                            }
                        }
                        else if(echoDisplayMode.hasFocus){
                            if(app.echoDisplayModeValue === 1){
                                app.echoDisplayModeValue = 0
                                ft2232HWrapper.wrechoDisplayMode(app.echoDisplayModeValue)
                            }
                        }
                        else if(echoFreeze.hasFocus){
                            if(app.echoFreezeValue === 1){
                                app.echoFreezeValue = 0
                                ft2232HWrapper.wrechoFreeze(app.echoFreezeValue)
                            }
                        }
                        else if(batteryRecord.hasFocus){
                            if(app.batteryRecordValue === 1){
                                app.batteryRecordValue = 0
                                adctimer.stop()
//                                appManager.writeFooter("/opt/battery.txt")
                                status.text = "stopped"
//                                ft2232HWrapper.wrechoFreeze(app.echoFreezeValue)
                            }
                        }
                        //
                    }
                    else if(event.key === Utils.KEY_BACK){   // now use this ESC button to exit application
                        ft2232HWrapper.closeFt2232H()
                        appManager.startApp("/opt/Launcher/bin/Launcher")
                    }
                    else if(event.key === Utils.KEY_POWER){
                        // todo: save the params before power off

                        // popup a power off confirm window
                        poweroffdlg.visible = true
                        poweroffdlg.focus = true
                    }

                    // test battery record download
                    else if(event.key === Utils.KEY_4){   // download battery.txt file to usb disk
                        var usbnode = appManager.getUsbDiskNode()
                        if(usbnode === "no usb"){
                            status.text = "no usb"
                            return
                        }
                        appManager.downloadFileToUsbDisk(usbnode, "/opt/battery.txt")
                        status.text = "download battery.txt at usb disk root directory"
                    }

                    console.log("app.focusItemIndex: ", app.focusItemIndex)
                }//Keys.onPressed

                Keys.onReleased: {
                    long_press_timer.stop()
                    change_value_timer.stop()
                }//Keys.onReleased

                Column{
                    id: menu_1
                    width: 128
                    spacing: 0
                    height: parent.height
                    visible: app.focusItemIndex < 5

                    FunctionButton {
                        id: gain
                        title: qsTr("menu_gain")
                        value: app.gainValue
                        index: 0
                        // for now, gain's hasFocus is true[just for test period]
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }
                    }
                    FunctionButton{
                        id: range
                        title: qsTr("menu_range")
                        value: app.rangeValue
                        index: 1
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: high_vol
                        title: qsTr("menu_high_vol")
                        value: {
                            if(app.highVolValue === 0) return "50"
                            else if(app.highVolValue === 1) return "100"
                            else if(app.highVolValue === 3) return "200"
                            else if(app.highVolValue === 7) return "400"
                        }
                        index: 2
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: emit_damp
                        title: qsTr("menu_damp")
                        value: app.emitDampValue === 0 ? qsTr("low_damp") : qsTr("high_damp")
                        index: 3
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: filter_band
                        title: qsTr("menu_filter_band")
                        value: {
                            if(app.filterBandValue === 0)return "0.5-20MHz"
                            else if(app.filterBandValue === 2)return "2-8MHz"
                            else if(app.filterBandValue === 3)return "0.5-4MHz"
                        }
                        index: 4
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                } //Column end

                Column{
                    id: menu_2
                    width: 128
                    spacing: 0
                    height: parent.height
                    visible: app.focusItemIndex > 4 && app.focusItemIndex < 10

                    FunctionButton {
                        id: rectificationType
                        title: qsTr("menu_rectificationType")
                        value: {
                            if(app.rectificationTypeValue === 0) return qsTr("rectification_radio")
                               else if(app.rectificationTypeValue === 1) return qsTr("rectification_all")
                               else if(app.rectificationTypeValue === 2) return qsTr("rectification_plus")
                               else if(app.rectificationTypeValue === 3) return qsTr("rectification_minus")
                        }
                        index: 5
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: transducerType
                        title: qsTr("menu_transducerType")
                        value:{
                            if(app.transducerTypeValue === 0) return qsTr("transducer_single")
                            else if(app.transducerTypeValue === 1) return qsTr("transducer_double")

                        }
                        index: 6
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: tcgGain
                        title: qsTr("menu_tcgGain")
                        value: {
                            if(app.tcgGainValue === 0) return qsTr("tcg_off")
                            else if(app.tcgGainValue === 1) return qsTr("tcg_on")

                        }
                        index: 7
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: keyBoardBacklight
                        title: qsTr("menu_keyBoardBacklight")
                        value: {
                            if(app.keyBoardBacklightValue === 0)return qsTr("light_off")
                            else if(app.keyBoardBacklightValue === 1)return qsTr("light_low")
                            else if(app.keyBoardBacklightValue === 2)return qsTr("light_mid")
                            else if(app.keyBoardBacklightValue === 3)return qsTr("light_high")
                        }

                        index: 8
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton{
                        id: vga
                        title: qsTr("menu_vga")
                        value: {
                            if(app.vgaValue === 0)return qsTr("vga_off")
                            else if(app.vgaValue === 1)return qsTr("vga_on")

                        }
                        index: 9
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                } //Column end

                Column{
                    id: menu_3
                    width: 128
                    spacing: 0
                    height: parent.height
                    visible: app.focusItemIndex >= 10

                    FunctionButton {
                        id: repeatFreq
                        title: qsTr("menu_repeatFreq")
                        value: app.repeatFreqValue
                        index: 10
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton {
                        id: hardwareDraw
                        title: qsTr("menu_hardwareDraw")
                        value: {
                            if(app.hardwareDrawValue === 0)return qsTr("off")
                            else if(app.hardwareDrawValue === 1)return qsTr("on")
                        }
                        index: 11
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton {
                        id: echoDisplayMode
                        title: qsTr("menu_echoDisplayMode")
                        value: {
                            if(app.echoDisplayModeValue === 0)return qsTr("hollow")
                            else if(app.echoDisplayModeValue === 1)return qsTr("solid")
                        }
                        index: 12
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    FunctionButton {
                        id: echoFreeze
                        title: qsTr("menu_echoFreeze")
                        value: {
                            if(app.echoFreezeValue === 0)return qsTr("off")
                            else if(app.echoFreezeValue === 1)return qsTr("on")
                        }
                        index: 13
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    // test battery record
                    FunctionButton {
                        id: batteryRecord
                        title: qsTr("Bat Rec")
                        value: {
                            if(app.batteryRecordValue === 0)return qsTr("off")
                            else if(app.batteryRecordValue === 1)return qsTr("on")
                        }
                        index: 14
                        hasFocus: {
                            if(app.focusItemIndex === index) return true
                            else return false
                        }

                    }
                    // no FunctionButton in menu 3


                } //Column end

            }



        }
        Rectangle{
            width: parent.width
            height: 20
            color: "blue"
            Text {
                id: status
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.pixelSize: 20
                text: app.info
            }
        }
    }

    // up most
    PoweroffDialog{
        id: poweroffdlg
        visible: false
        onConfirmed: {

            console.log("-------confirm")
            poweroffdlg.focus = false
            menu_container.focus = true
            poweroffdlg.visible = false
            appManager.powerOff()
        }
        onCancelled: {
            console.log("-------cancel")
            poweroffdlg.focus = false
            menu_container.focus = true
            poweroffdlg.visible = false

        }
    }


}
