Couple of notes to help debug the local start-up services
### Check Grafana connectivity
- sudo systemctl status pdc.service
Should have something like this:
```
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: read<=0 rfd 4 len 0
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: read failed
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: chan_shutdown_read (i0 o0 sock -1 wfd 4 efd 6 [write])
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: input open -> drain
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: ibuf empty
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: send eof
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel 0: input drain -> closed
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: channel_input_status_confirm: type 99 id 0
Sep 23 11:39:14 raspberrypi sh[295229]: debug2: shell request accepted on channel 0
Sep 23 11:39:14 raspberrypi sh[295229]: This is Grafana Private Datasource Connect!
```

Restart it if there is an error.

## Four system service files
In /etc/systemd/system/

aqi_pi.service
```[Unit]
Description=AQI and Temp/RH Sensor Monitor (Python Script)
After=network.target

[Service]
ExecStart=/usr/bin/python3 main_prometheus.py
WorkingDirectory=/home/bernp/sourcecode/particle/pi
StandardOutput=inherit
StandardError=inherit
Restart=always
User=bernp

[Install]
WantedBy=multi-user.target
```

prometheus.service
```
[Unit]
Description=AQI and Temp/RH Sensor Monitor (Python Script)
After=network.target

[Service]
ExecStart=/usr/bin/python3 main_prometheus.py
WorkingDirectory=/home/bernp/sourcecode/particle/pi
StandardOutput=inherit
StandardError=inherit
Restart=always
User=bernp

[Install]
WantedBy=multi-user.target
bernp@raspberrypi:/etc/systemd/system $ cat prometheus.service
[Unit]
Description=Prometheus Server
Documentation=https://prometheus.io/docs/introduction/overview/
After=network-online.target

[Service]
User=bernp
Restart=never

ExecStart=/home/bernp/prometheus/prometheus \
  --config.file=/home/bernp/prometheus/prometheus.yml \
  --storage.tsdb.path=/home/bernp/prometheus/data \
  --web.enable-remote-write-receiver \
  --log.level=debug

[Install]
WantedBy=multi-user.target
```

relay_prometheus.service
```
[Unit]
Description=Prometheus Server
Documentation=https://prometheus.io/docs/introduction/overview/
After=network-online.target

[Service]
User=bernp
Restart=never

ExecStart=/home/bernp/prometheus/prometheus \
  --config.file=/home/bernp/prometheus/prometheus.yml \
  --storage.tsdb.path=/home/bernp/prometheus/data \
  --web.enable-remote-write-receiver \
  --log.level=debug

[Install]
WantedBy=multi-user.target
bernp@raspberrypi:/etc/systemd/system $ cat relay_prometheus.service
[Unit]
Description=REST API relay server for prometheus (Python Script)
After=network.target

[Service]
ExecStart=/usr/bin/python3 relay_prometheus.py
WorkingDirectory=/home/bernp/sourcecode/particle/pi
StandardOutput=inherit
StandardError=inherit
Restart=always
User=bernp

[Install]
WantedBy=multi-user.target
```

pdc.service
```
[Unit]
Description=Private Datasource Connect Agent
After=network-online.target
RestartSec=5

[Service]
ExecStart=/bin/sh -c /home/bernp/grafana-pdc/start_pdc
WorkingDirectory=/home/bernp/grafana-pdc
StandardOutput=inherit
StandardError=inherit
Restart=always
User=bernp

[Install]
WantedBy=multi-user.target
```

