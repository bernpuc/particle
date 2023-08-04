import subprocess

def get_percent_uptime():
    try:
        result = subprocess.run(['uprecords'], capture_output=True, text=True)
        output = result.stdout.strip()

        # Find the line that contains the percent uptime
        for line in output.split('\n'):
            if "%up" in line:
                percent_uptime = line.split()[1]
                return percent_uptime

    except Exception as e:
        print("Error:", e)

    return None

if __name__ == "__main__":
    percent_uptime = get_percent_uptime()
    if percent_uptime is not None:
        print("Percent Uptime:", percent_uptime)
    else:
        print("Unable to retrieve percent uptime.")
