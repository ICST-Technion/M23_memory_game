import 'dart:convert';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:flutter/material.dart';
import 'package:gsheets/gsheets.dart';


const _credentials = r'''
  {
  "type": "service_account",
  "project_id": "shimon-399418",
  "private_key_id": "ff842607a1b9fd5cc28c20928c0cb6114a21bebb",
  "private_key": "-----BEGIN PRIVATE KEY-----\nMIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDQ7cT9rX8GWFbk\niyfqzn+eTsp8GcESmlbIn3szThrJBFCwyh3BV2xjMwSIoBfX1OdrG0bzCugRpTtn\nm0ji2+MMxxnyYi95dKgZlbKEMWDMgguxcMRvMXZsl5pCoRfvBhjNXLhKiXZ9W01K\nwYOKnuXsV/RV8Fz12lI4MR9PgDfkrPJdc4B1lR4l1SVqmfc0LMkxNFnIZzzuqr72\nvFRk6gw85gNKr0hSEU+g0TD0RCNnW+xGm8gn93ytrC4gcUgLHq1nF8tsEdJsqZZj\nn5PVp4a22+YKWyVJOM+5VEz9LsIOJb/mH1jOX/i5UzBIBl3KK8PttVrsR4sBAAIw\neyktFsKvAgMBAAECggEAAI/FufQs8BHtuIiNOhWFr0xr6+slIzfUqDkRhjZICSag\npuZW2nhchg8S4eEi9Jq8Z9kiFtt+ezRUAA0hTtwjdJsrvS85YcddVPrw3hJvPa19\n0DjK4Qz40gWS+GnhcWw1RdwMHMXY0oZxIz8jljb4zEACRqGHm8Y99qu0d1doubh3\nYN9/xyShJU7HqCuKFEP8WQgpKYYJ4o/QAF3/jmIlJunQo85SuCloYHXIFyO7FzkP\npA6CPmnOG+E/epIK/bdMp3Klqfj4w5PaFKd3NTs7VvaRQEI3OhLjqw2m11FTYmCK\nGURWAAGTgLcVqG7s4I49Ugq53iKLCCiN+j566Gm5vQKBgQD7wX635/Xy6V2g9iG7\nUSH7FQFekIrxDIJRCaUfI6GT6CSc9851YvNaYKkRRLzGHTLJG1sCw8xa4fTDfDer\nbrVbJUg1Vea0raZV9hjFR8VuztxksyxhMU3kIg/sVQsw0AkNtrnh83BPYB7OTo7f\nrYX38GdHF46ePfhyN6S1M+c78wKBgQDUc3IFYTFG8AqUwTPQosQpZFxbWAXYNemb\n7JtQy4app2RRzbmNNDM3E9jaSk8VLtBzKhEbPdSi2+jJmoF0RcSTSlWZi62Ox5A4\nqzgNPRPh2w33yNIGwr0JONSjEApT6i5pUtoOdNyD3obp0d43azD/KwMg3oouWJSQ\ngb1JQp55VQKBgB0s+G2BsUwcZWBhBdm6dlCtx4HZCHEU/Qk9L+ii7XfBoHScd029\nUOx5ZT5+E8511cveorZ4UHd71izNeXohk1zjvjGIKftoYG133rkGL0g/KuV0qkD9\nQMSeee/ujyRyB4qSUYpw9uQMBKXuDrq3Vl48ewuPHFUUY3WBI1JomHSnAoGAaYHn\n11ocYdFmP9fvRqb+6luILmj5hhXILYKE48wf1XNnM+UKSGDrKmg14rrSqX91UCxN\nEI1VY4f5BN4xejAH3uoktruSdv0O18XYcWKq1Bi4yBy4iVj/Dmxl1ZFqv5/marIV\nzuYkeWzy7X4ncLhQPfj7dcBvOXMOXPHBsndeQn0CgYEAs5919ilsujZzzTfWZCQ+\nvQYKRm80gUJ4434xxjPMF6szy7DEuNzEjhKNVAd4Zt6F5/V+wxpCR10wMPiyJ2bQ\nfquyU+OMiPyqS3GhDHUVnWbgU7uMoxuvH7jGv6JEMOQBwebrctxlmDV41D41PQcN\nX7Y4ALra4tF/d02LqopeGds=\n-----END PRIVATE KEY-----\n",
  "client_email": "shimon@shimon-399418.iam.gserviceaccount.com",
  "client_id": "114150410882351453785",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/shimon%40shimon-399418.iam.gserviceaccount.com",
  "universe_domain": "googleapis.com"
}
''';

const _spreadsheetId = "1hpwi6DFA_93WcVHHwyKn_Oi6hVpdILWfb6wM6EAUJ-I";
//void main() => runApp(MyApp());
void main() async {
  // final gsheets = GSheets(_credentials);
  // final ss = await gsheets.spreadsheet(_spreadsheetId);
  // var sheet = ss.worksheetByTitle("sheet1");
  // await sheet!.values.insertValue("JOJO", column: 1, row: 9);
  // var scores = await sheet!.values.allRows();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Bluetooth Flutter App',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  BluetoothConnection? connection;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Stack(
        children: <Widget>[
          Image.asset(
            "assets/white.jpeg",
            width: double.infinity,
            height: double.infinity,
            fit: BoxFit.cover,
          ),
          Center(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                Image.asset(
                  "assets/ham.png",
                  width: 200,
                  height: 200,
                ),
                ElevatedButton(
                  onPressed: _connect,
                  child: Text('Find Device'),
                )
              ],
            ),
          ),
        ],
      ),
    );
  }

  void _connect() async {
    BluetoothDevice? selectedDevice = await Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => DeviceListPage()),
    );

    if (selectedDevice != null) {
      try {
        connection = await BluetoothConnection.toAddress(selectedDevice.address);
        print('Connected to ${selectedDevice.name}');

        if (connection != null) {
          final enteredName = await Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => EnterNameScreen()),
          );
          if (enteredName != null) {
            Navigator.push(
              context,
              MaterialPageRoute(builder: (context) => CheckScreen(connection: connection!, userName: enteredName)),
            );
          }
        }

      } catch (e) {
        print('Error connecting: $e');
      }
    }
  }
}

class DeviceListPage extends StatelessWidget {
  Future<List<BluetoothDevice>> _getBondedDevices() async {
    List<BluetoothDevice> devices = [];
    try {
      devices = await FlutterBluetoothSerial.instance.getBondedDevices();
    } catch (e) {
      print('Error getting bonded devices: $e');
    }
    return devices;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Select Device'),
      ),
      body: FutureBuilder<List<BluetoothDevice>>(
        future: _getBondedDevices(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            return Center(child: Text('Error: ${snapshot.error}'));
          } else if (!snapshot.hasData || snapshot.data!.isEmpty) {
            return Center(child: Text('No bonded devices found.'));
          } else {
            return ListView.builder(
              itemCount: snapshot.data!.length,
              itemBuilder: (context, index) {
                BluetoothDevice device = snapshot.data![index];
                return ListTile(
                  title: Text(device.name ?? 'Unknown Name'),
                  subtitle: Text(device.address ?? 'Unknown Address'),
                  onTap: () {
                    Navigator.pop(context, device);
                  },
                );
              },
            );
          }
        },
      ),
    );
  }
}

class EnterNameScreen extends StatefulWidget {
  @override
  _EnterNameScreenState createState() => _EnterNameScreenState();
}

class _EnterNameScreenState extends State<EnterNameScreen> {
  final TextEditingController _nameController = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('Enter Your Name')),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          children: <Widget>[
            TextField(
              controller: _nameController,
              maxLength: 15,
              decoration: InputDecoration(labelText: 'Name'),
            ),
            ElevatedButton(
              onPressed: () {
                final enteredName = _nameController.text.trim();
                if (enteredName.length >= 2) {
                  Navigator.pop(context, enteredName);
                } else {
                  ScaffoldMessenger.of(context).showSnackBar(
                    SnackBar(content: Text('Name should be at least 2 characters long')),
                  );
                }
              },
              child: Text('Save Name'),
            ),
          ],
        ),
      ),
    );
  }
}

class CheckScreen extends StatefulWidget {
  final BluetoothConnection connection;
  final String userName;

  CheckScreen({required this.connection, required this.userName});

  @override
  _CheckScreenState createState() => _CheckScreenState();
}

class _CheckScreenState extends State<CheckScreen>  {
  int? elapsedTime; // New state variable for elapsed time
  String stage="0";



  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Hello, ${widget.userName}'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text('Current Stage: ${elapsedTime ?? 'Not available'}'), // Display elapsed time
            ElevatedButton(
              onPressed: _startGame,
              child: Text('Start Game'),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () => _showSettings(context),
              child: Text('Settings'),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: _showTopScores,
              child: Text('Top Scores'),
            ),
          ],
        ),
      ),
    );
  }

  // void _startGame() {
  //   if (widget.connection.isConnected) {
  //     Uint8List data = Uint8List.fromList(utf8.encode('start_game'));
  //     widget.connection.output.add(data);
  //     widget.connection.output.allSent.then((_) {
  //       print('start_game');
  //
  //       // String received = ascii.decode(data).trim();
  //       // if (received.startsWith('end_game_time ')) {
  //       //   setState(() {
  //       //     elapsedTime = int.tryParse(received.substring(14));
  //       //   });
  //       // }
  //     });
  //   } else {
  //     print('Not connected');
  //   }
  // }

  void _startGame()  {
    String stage= "";
    if (widget.connection.isConnected) {
      Uint8List data = Uint8List.fromList(utf8.encode('start_game'));
      widget.connection.output.add(data);
      widget.connection.output.allSent.then((_) {
        print('start_game');
      });


      // Listen for incoming data
      widget.connection.input?.listen((Uint8List receivedData) {
        String received = utf8.decode(receivedData).trim();

        if (received.startsWith('game_stage ')) {
          setState(() {
            stage = received.substring(11);
            elapsedTime = int.tryParse(received.substring(11));
            //stage = elapsedTime as String;
            if (elapsedTime != null) {
              // Process the elapsedTime as needed
              print('Received game stage: $elapsedTime');
            }
          });
        }
        if (received.startsWith('reset') || received.startsWith('end_game_time')) {
          setState(()  {
            if (elapsedTime != null) {
              // Process the elapsedTime as needed
              print('Received game stage: $elapsedTime');

            if (received.startsWith('end_game_time')){
              updateTable(stage);
            }
              elapsedTime = 0;
              stage="0";


/*          final gsheets= GSheets(_credentials);
            final ss = await gsheets.spreadsheet(_spreadsheetId);
            var sheet = ss.worksheetByTitle("sheet1");
            await sheet!.values.insertValue( int.tryParse(received.substring(11)) as Object, column: 2, row: 3);
            await sheet!.values.insertValue( widget.userName, column: 1, row: 3);*/

            }
          });
        }
        if (received.startsWith('hard_reset')){
          stage="0";
          elapsedTime=0;
          Navigator.push(
            context,
            MaterialPageRoute(builder: (context) => MyHomePage()),
          );
        }
      });
    } else {
      print('Not connected');
    }
  }

  Future<void> updateTable(String stage)
  async {
    final gsheets= GSheets(_credentials);
    final ss = await gsheets.spreadsheet(_spreadsheetId);
    var sheet = ss.worksheetByTitle("sheet1");
    List<String> new_row = [widget.userName, stage];
    await sheet!.values.appendRow(new_row);
    //await sheet!.values.insertValue( widget.userName, column: 1, row: 4);
    //await sheet!.values.insertValue( stage, column: 2, row: 4);


  }



  void _showTopScores() async{
    final gsheets= GSheets(_credentials);
    final ss = await gsheets.spreadsheet(_spreadsheetId);
    var sheet = ss.worksheetByTitle("sheet1");
    var scores_all = await sheet!.values.allRows();
    scores_all.sort((a, b) => int.parse(b[1]).compareTo(int.parse(a[1])));
    List<List<dynamic>> scores = [];

    // Add the first 10 couples of scores to the "shahar" list
    for (int i = 0; i < 10 && i < scores_all.length; i++) {
      scores.add(scores_all[i]);
    }



    Navigator.push(
      context,
      MaterialPageRoute(builder: (context) => TopScoresScreen(scores: scores)),
    );
  }

  void _showSettings(BuildContext context) {
    bool isSlowMode = false;
    bool is3light = false;
    String? selectedSoundSet = 'sound_set_1'; // Default value
    String? selectedColorSet = 'color_set_1'; // Default value

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Settings'),
          content: StatefulBuilder(
            builder: (BuildContext context, StateSetter setState) {
              return Column(
                mainAxisSize: MainAxisSize.min,
                children: <Widget>[
                  SwitchListTile(
                    title: Text('Slow Mode'),
                    value: isSlowMode,
                    onChanged: (bool value) {
                      setState(() {
                        isSlowMode = value;
                      });
                    },
                  ),
                  SwitchListTile(
                    title: Text('3 lights mode'),
                    value: is3light,
                    onChanged: (bool value) {
                      setState(() {
                        is3light = value;
                      });
                    },
                  ),
                  SizedBox(height: 16.0),
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Text(
                      'Choose Sound',
                      style: TextStyle(
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                  ),
                  ListTile(
                    title: Text('Sound Set 1'),
                    leading: Radio(
                      value: 'sound_set_1',
                      groupValue: selectedSoundSet,
                      onChanged: (String? value) {
                        setState(() {
                          selectedSoundSet = value;
                        });
                      },
                    ),
                  ),
                  ListTile(
                    title: Text('Sound Set 2'),
                    leading: Radio(
                      value: 'sound_set_2',
                      groupValue: selectedSoundSet,
                      onChanged: (String? value) {
                        setState(() {
                          selectedSoundSet = value;
                        });
                      },
                    ),
                  ),
                  ListTile(
                    title: Text('Const sound'),
                    leading: Radio(
                      value: 'sound_set_const',
                      groupValue: selectedSoundSet,
                      onChanged: (String? value) {
                        setState(() {
                          selectedSoundSet = value;
                        });
                      },
                    ),
                  ),
                  SizedBox(height: 16.0),
                  Align(
                    alignment: Alignment.centerLeft,
                    child: Text(
                      'Choose Color',
                      style: TextStyle(
                        fontWeight: FontWeight.bold,
                      ),
                    ),
                  ),
                  ListTile(
                    title: Text('Color Set 1'),
                    leading: Radio(
                      value: 'color_set_1',
                      groupValue: selectedColorSet,
                      onChanged: (String? value) {
                        setState(() {
                          selectedColorSet = value;
                        });
                      },
                    ),
                  ),
                  ListTile(
                    title: Text('Color Set 2'),
                    leading: Radio(
                      value: 'color_set_2',
                      groupValue: selectedColorSet,
                      onChanged: (String? value) {
                        setState(() {
                          selectedColorSet = value;
                        });
                      },
                    ),
                  ),
                  ListTile(
                    title: Text('Color Set 3'),
                    leading: Radio(
                      value: 'color_set_3',
                      groupValue: selectedColorSet,
                      onChanged: (String? value) {
                        setState(() {
                          selectedColorSet = value;
                        });
                      },
                    ),
                  ),
                ],
              );
            },
          ),
          actions: <Widget>[
            TextButton(
              onPressed: () {
                _sendSettings(isSlowMode, is3light, selectedSoundSet, selectedColorSet);
                Navigator.of(context).pop(); // Close the settings dialog
              },
              child: Text('Save Settings'),
            ),
          ],
        );
      },
    );
  }

  void _sendSettings(bool isSlowMode, bool is3light, String? selectedSoundSet, String? selectedColorSet) {
    String str = '';

    if (!isSlowMode) {
      str += 'fast_mode ';
    } else {
      str += 'slow_mode ';
    }

    if (!is3light) {
      str += '4_lights ';
    } else {
      str += '3_lights ';
    }

    str += '$selectedSoundSet $selectedColorSet';
    if (widget.connection.isConnected) {
      Uint8List data = Uint8List.fromList(utf8.encode(str));
      widget.connection.output.add(data);
      widget.connection.output.allSent.then((_) {
        print(str);
      });
    } else {
      print('Not connected');
    }
  }
}



// class TopScoresScreen extends StatelessWidget {
//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(
//         title: Text('Top Scores'),
//       ),
//       body: Center(
//         child: Text('Top Scores Screen Content'),
//       ),
//     );
//   }
// }

class TopScoresScreen extends StatelessWidget {
  final List<List<dynamic>> scores;

  TopScoresScreen({required this.scores});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Top Scores'),
      ),
      body: ListView.builder(
        itemCount: scores.length,
        itemBuilder: (context, index) {
          final scoreRow = scores[index];
          return ListTile(
            title: Text('Name: ${scoreRow[0]}'),
            subtitle: Text('Score: ${scoreRow[1]}'),
          );
        },
      ),
    );
  }
}
