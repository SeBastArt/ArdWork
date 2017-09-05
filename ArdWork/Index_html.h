static String GetIndexHtml() {
	String html;
	html = "";

	html += "<!DOCTYPE html> \n";
	html += "<html> \n";
	html += "<head> \n";
	html += "    <title>Bilderrahme</title> \n";

	html += "    <meta charset=\"utf-8\"> \n";
	html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> \n";

	html += "    <!-- SCRIPT --> \n";
	html += "    <script src=\"dist/js/jquery.min.js\"></script> \n";
	html += "    <script src=\"dist/js/kube.min.js\"></script> \n";
	html += "    <script src=\"dist/js/jscolor.min.js\"></script> \n";
	html += "    <script src=\"dist/js/chartist.min.js\"></script> \n";

	html += "    <!--  CSS --> \n";
	html += "    <link rel=\"stylesheet\" href=\"dist/css/chartist.min.css\"> \n";
	html += "    <link rel=\"stylesheet\" href=\"dist/css/kube.min.css\"> \n";
	html += "    <link rel=\"stylesheet\" href=\"dist/css/bttn.min.css\"> \n";


	html += "    <script> \n";
	html += "        function update() { \n";
	html += "            var data = { \n";
	html += "                labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri'], \n";
	html += "                series: [ \n";
	html += "                    [5, 2, 4, 2, 0] \n";
	html += "                ] \n";
	html += "            }; \n";

	html += "            new Chartist.Line('.chart1', data); \n";
	html += "            new Chartist.Bar('.chart2', data); \n";
	html += "            new Chartist.Line('.chart3', data); \n";
	html += "        } \n";
	html += "    </script> \n";
	html += "    <style> \n";
	html += "        body { \n";
	html += "            font-family: Tahoma, Verdana, Segoe, sans-serif; \n";
	html += "            margin-left: 10%; \n";
	html += "            margin-right: 10%; \n";
	html += "            background-color: #DFFFDF; \n";
	html += "            min-width: 400px; \n";
	html += "        } \n";

	html += "        .content { \n";
	html += "            padding-left: 10px; \n";
	html += "            padding-right: 10px; \n";
	html += "        } \n";

	html += "        .simple-text { \n";
	html += "            color: #e3e3e3; \n";
	html += "            text-shadow: 1px 1px 0px black; \n";
	html += "        } \n";

	html += "        a { \n";
	html += "            text-align: center; \n";
	html += "            text-decoration: none; \n";
	html += "            display: inline-block; \n";
	html += "        } \n";

	html += "            a.hover { \n";
	html += "                text-decoration: none; \n";
	html += "            } \n";

	html += "        .tabs { \n";
	html += "            background: #e3e3e3; \n";
	html += "            background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); \n";
	html += "            background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); \n";
	html += "        } \n";

	html += "            .tabs a { \n";
	html += "                width: 100%; \n";
	html += "                background: transparent; \n";
	html += "                padding: 10px 25px; \n";
	html += "                position: center; \n";
	html += "            } \n";

	html += "                .tabs a:hover { \n";
	html += "                    background: white; \n";
	html += "                    text-decoration: none; \n";
	html += "                } \n";

	html += "                .tabs a:visited { \n";
	html += "                    background: white; \n";
	html += "                } \n";

	html += "            .tabs h3 { \n";
	html += "                font-family: \"Century Gothic\", CenturyGothic, Geneva, AppleGothic, sans-serif; \n";
	html += "                font-size: 1.3em; \n";
	html += "                line-height: 1.5em; \n";
	html += "                margin: 0.3em 0.7em 0.3em 0.7em; \n";
	html += "                letter-spacing: -0.1em; \n";
	html += "                color: #454545; \n";
	html += "                text-align: center; \n";
	html += "            } \n";

	html += "            .tabs li { \n";
	html += "                font-size: 1.2em; \n";
	html += "            } \n";

	html += "        .container { \n";
	html += "            background: #666; \n";
	html += "            margin: auto; \n";
	html += "            padding-top: 30px; \n";
	html += "            font-family: helvetica, arial, sans-serif; \n";
	html += "        } \n";

	html += "        .head-line { \n";
	html += "            background: #e3e3e3; \n";
	html += "            position: relative; \n";
	html += "            background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); \n";
	html += "            background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); \n";
	html += "            padding: 10px 20px; \n";
	html += "            margin-left: -20px; \n";
	html += "            margin-top: 0; \n";
	html += "            width: 70%; \n";
	html += "            max-width: 550px; \n";
	html += "            min-width: 400px; \n";
	html += "            font-family: Georgia, 'Times New Roman', serif; \n";
	html += "            -moz-box-shadow: 1px 1px 3px #292929; \n";
	html += "            -webkit-box-shadow: 1px 1px 3px #292929; \n";
	html += "            color: #454545; \n";
	html += "            text-shadow: 0 1px 0 white; \n";
	html += "        } \n";

	html += "            .head-line.small { \n";
	html += "                width: 30%; \n";
	html += "                min-width: 200px; \n";
	html += "                max-width: 250px; \n";
	html += "                font-size: 1.3em; \n";
	html += "                line-height: 0.8em; \n";
	html += "                font-family: Tahoma, Verdana, Segoe, sans-serif; \n";
	html += "            } \n";

	html += "        .arrow { \n";
	html += "            position: absolute; \n";
	html += "            width: 0; \n";
	html += "            height: 0; \n";
	html += "            line-height: 0; \n";
	html += "            border-left: 20px solid transparent; \n";
	html += "            border-top: 10px solid #c8c8c8; \n";
	html += "            top: 104%; \n";
	html += "            left: 0; \n";
	html += "        } \n";

	html += "        .button { \n";
	html += "            font-size: 18px; \n";
	html += "            font-weight: bold; \n";
	html += "            text-decoration: none; \n";
	html += "            background: #e3e3e3; \n";
	html += "            position: relative; \n";
	html += "            background: -moz-linear-gradient(top, #e3e3e3, #c8c8c8); \n";
	html += "            background: -webkit-gradient(linear, left top, left bottom, from(#e3e3e3), to(#c8c8c8)); \n";
	html += "            font-family: Georgia, 'Times New Roman', serif; \n";
	html += "            -moz-box-shadow: 1px 1px 3px #292929; \n";
	html += "            -webkit-box-shadow: 1px 1px 3px #292929; \n";
	html += "            color: #454545; \n";
	html += "            text-shadow: 0 1px 0 white; \n";
	html += "            align: left; \n";
	html += "        } \n";

	html += "            .button:hover { \n";
	html += "                background-color: #454545; \n";
	html += "                color: #454545; \n";
	html += "                box-shadow: 0 1px 5px #313131, 0 3px 3px #393939, 0 3px 10px rgba(0, 0, 0, 0.1), 0 2px 15px rgba(200, 200, 200, 0.4) inset, 0 -2px 9px rgba(0, 0, 0, 0.2) inset; \n";
	html += "            } \n";


	html += "            .button:active { \n";
	html += "                box-shadow: 0 2px 0 #393939, 0 4px 4px rgba(0, 0, 0, 0.4), 0 2px 5px rgba(0, 0, 0, 0.2) inset; \n";
	html += "                top: 2px; \n";
	html += "            } \n";

	html += "        .desc { \n";
	html += "            color: #454545; \n";
	html += "        } \n";
	html += "    </style> \n";
	html += "</head> \n";
	html += "<body onload=\"update()\"> \n";

	html += "    <div class=\"container\"> \n";
	html += "        <h2 class=\"head-line\">Bilderrahmen Steuerung <span class=\"arrow\"></span> </h2> \n";
	html += "        <div data-component=\"sticky\"> \n";
	html += "            <nav class=\"tabs\" data-component=\"tabs\"> \n";
	html += "                <ul onclick=\"update()\"> \n";
	html += "                    <li><h3><strong>ArdWork</strong></h3></li> \n";
	html += "                    <li class=\"active\"><a href=\"#tabWifi\">Wifi</a></li> \n";
	html += "                    <li><a href=\"#tabStripe\">RGB-Stripe</a></li> \n";
	html += "                    <li><a href=\"#tabLuxmeter\">Luxmeter</a></li> \n";
	html += "                    <li><a href=\"#tabDebug\">Debug</a></li> \n";
	html += "                </ul> \n";
	html += "            </nav> \n";
	html += "        </div> \n";


	html += "        <div class=\"content\" id=\"tabWifi\"> \n";
	html += "            <div class=\"simple-text\"> \n";
	html += "                <p> \n";
	html += "                    <strong> \n";
	html += "                        Wifi stellt die Verbindung zum heimischen Netzwerk her \n";
	html += "                        oder stellt selbst einen AcessPoint bereit \n";
	html += "                    </strong> \n";
	html += "                </p> \n";

	html += "                <div class=\"head-line small\">Features</div> \n";
	html += "                <p> \n";
	html += "                    <ul> \n";
	html += "                        <li>SSID ändern</li> \n";
	html += "                        <li>Passwort ändern</li> \n";
	html += "                        <li>Wechsel der Modi STA/AP</li> \n";
	html += "                        <li>Reconnect</li> \n";
	html += "                    </ul> \n";
	html += "                </p> \n";
	html += "            </div> \n";
	html += "            <div class=\"head-line small\">Steuerung</div> \n";
	html += "            <br> \n";
	html += "            <p id=\"my_p\"> \n";
	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm small\"> \n";
	html += "                            <input type=\"text\" name=\"Wifi-SSID\" class=\"small\" placeholder=\"SSID\"> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Set SSID</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm small\"> \n";
	html += "                            <input type=\"password\" name=\"Wifi-Password\" class=\"small\" placeholder=\"Password\"> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Set Password</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option>STA</option> \n";
	html += "                                <option>AP</option> \n";
	html += "                                <option>STA/AP</option> \n";
	html += "                            </select> \n";

	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Set Wifi-Mode</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <button class=\"button small\">Reconnect</button> \n";
	html += "                    </div> \n";
	html += "                </form> \n";
	html += "            </p> \n";
	html += "            <br> \n";
	html += "        </div> \n";

	html += "        <div class=\"content\" id=\"tabDebug\"> \n";
	html += "            <div class=\"simple-text\"> \n";
	html += "                <p> \n";
	html += "                    <strong> \n";
	html += "                        Debug stellt die Controlle über alle verbauten elemente bereit. \n";
	html += "                        <br>Diese Funktion muss erst aktiviert werden \n";
	html += "                    </strong> \n";
	html += "                </p> \n";
	html += "                <div class=\"head-line small\">Features</div> \n";
	html += "                <p> \n";
	html += "                    <ul> \n";
	html += "                        <li>LED1 an/aus/blink</li> \n";
	html += "                        <li>LED2 an/aus/blink</li> \n";
	html += "                        <li>Button1 Pressn</li> \n";
	html += "                        <li>Rohwert auslesen Luxmeter1</li> \n";
	html += "                    </ul> \n";
	html += "                </p> \n";
	html += "            </div> \n";
	html += "            <div class=\"head-line small\">Steuerung</div> \n";
	html += "            <br> \n";
	html += "            <p> \n";
	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option>Debug Off</option> \n";
	html += "                                <option>Debug On</option> \n";
	html += "                            </select> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Set Debug-Mode</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "            </p> \n";
	html += "        </div> \n";

	html += "        <div class=\"content\" id=\"tabStripe\"> \n";
	html += "            <div class=\"simple-text\"> \n";
	html += "                <p> \n";
	html += "                    <strong> \n";
	html += "                        RGB-Stripe stellt die Steuerung der RGB-LEDs bereit \n";
	html += "                        es erlaubt die Kontrolle über die Muster und Farben \n";
	html += "                    </strong> \n";
	html += "                </p> \n";
	html += "                <div class=\"head-line small\">RGB Features</div> \n";
	html += "                <p> \n";
	html += "                    <ul> \n";
	html += "                        <li>Pick a Pattern</li> \n";
	html += "                        <li>Pick a Color</li> \n";
	html += "                        <li>Turn On/OFF</li> \n";
	html += "                    </ul> \n";
	html += "                </p> \n";
	html += "            </div> \n";
	html += "            <div class=\"head-line small\">Steuerung</div> \n";
	html += "            <br> \n";
	html += "            <p> \n";
	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option value=\"0\">Cyclon</option> \n";
	html += "                                <option value=\"1\">Random</option> \n";
	html += "                                <option value=\"2\">Fire</option> \n";
	html += "                                <option value=\"3\">Shine</option> \n";
	html += "                            </select> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Pick a pattern</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <input value=\"e3e3e3\" class=\"small jscolor {mode:'HSV',width:243, height:150, position:'right', \n";
	html += "						borderColor:'#e3e3e3', insetColor:'#666', backgroundColor:'#666'}\"> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Pick a color</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option>On</option> \n";
	html += "                                <option>OFF</option> \n";
	html += "                            </select> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Set on/off</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";
	html += "            </p> \n";
	html += "        </div> \n";

	html += "        <div class=\"content\" id=\"tabLuxmeter\"> \n";
	html += "            <div class=\"simple-text\"> \n";
	html += "                <p> \n";
	html += "                    <strong> \n";
	html += "                        Luxmeter stellt die Steuerung des Lichtsensors bereit \n";
	html += "                        es erlaubt die Kontrolle über die Ausleseparameter \n";
	html += "                        und stellt Live-Werte sowie Diagramme bereit \n";
	html += "                    </strong> \n";
	html += "                </p> \n";
	html += "                <div class=\"head-line small\">Lux Features</div> \n";
	html += "                <p> \n";
	html += "                    <ul> \n";
	html += "                        <li>Fahrenheit/Celcius</li> \n";
	html += "                        <li>Integrationszeit</li> \n";
	html += "                        <li>Verstärkung(Gain)</li> \n";
	html += "                        <li>Lichtwert ausgeben</li> \n";
	html += "                    </ul> \n";
	html += "                </p> \n";
	html += "            </div> \n";
	html += "            <div class=\"head-line small\">Steuerung</div> \n";
	html += "            <br> \n";
	html += "            <p> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option>Celsius</option> \n";
	html += "                                <option>Fahrenheit</option> \n";
	html += "                            </select> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Pick a format</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option>13ms</option> \n";
	html += "                                <option>101ms</option> \n";
	html += "                                <option>402ms</option> \n";
	html += "                            </select> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Pick a integration time</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";

	html += "                <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "                    <div class=\"form-item\"> \n";
	html += "                        <div class=\"append w45 w100-sm\"> \n";
	html += "                            <select class=\"small\"> \n";
	html += "                                <option>Auto</option> \n";
	html += "                                <option>1x</option> \n";
	html += "                                <option>16x</option> \n";
	html += "                            </select> \n";
	html += "                            <button class=\"button small\">Set</button> \n";
	html += "                        </div> \n";
	html += "                        <div class=\"desc\">Pick a Gain</div> \n";
	html += "                    </div> \n";
	html += "                </form> \n";
	html += "                <br> \n";
	html += "                <center><strong>Helligkeitswerte der letzten 24h<strong></center> \n";
	html += "                <br> \n";
	html += "                <div class=\"chart1\" .ct-double-octave></div> \n";
	html += "                <div class=\"chart2\" .ct-double-octave></div> \n";
	html += "                <br> \n";
	html += "            </p> \n";
	html += "        </div> \n";

	html += "        <form method=\"post\" action=\"\" class=\"form\"> \n";
	html += "            <div class=\"form-item\"> \n";
	html += "                <div><button style=\"width:100%; margin-bottom: 2px;\" class=\"button small\">Save Config</button></div> \n";

	html += "            </div> \n";
	html += "        </form> \n";

	html += "    </div> \n";
	html += "</body> \n";
	html += "</html> \n";
}