import { useEffect, useState } from "react";
import "./App.css";

const API = "http://localhost:18080/api";

const DEMOS = {
  "NOP / HALT": {
    description: "Basic CPU execution demonstration.",
    start: 0x1000,
    bytes: [
      0x80,
      0x80,
      0x80,
      0x80,
      0x98,
    ],
  },

  "LED Counter": {
    description: "CPU drives the GPIO port.",
    start: 0x1000,
    bytes: [
      // MOV R0, 0x01
      0x81, 0x00, 0x00, 0x01, 0x01,

      // OUT 0xF000
      0x97, 0x02, 0xF0, 0x00,

      // MOV R0, 0x02
      0x81, 0x00, 0x00, 0x01, 0x02,

      // OUT 0xF000
      0x97, 0x02, 0xF0, 0x00,

      // MOV R0, 0x04
      0x81, 0x00, 0x00, 0x01, 0x04,

      // OUT 0xF000
      0x97, 0x02, 0xF0, 0x00,

      0x98,
    ],
  },

  "GPIO Pattern": {
    description: "Progressively lights the GPIO LEDs.",
    start: 0x1000,
    bytes: [
      0x81, 0x00, 0x00, 0x01, 0x01,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0x03,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0x07,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0x0F,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0x1F,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0x3F,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0x7F,
      0x97, 0x02, 0xF0, 0x00,

      0x81, 0x00, 0x00, 0x01, 0xFF,
      0x97, 0x02, 0xF0, 0x00,

      0x98,
    ],
  },

  "UART Hello": {
    description: "CPU sends serial data through UART.",
    start: 0x1000,
    bytes: [
      0x81, 0x00, 0x00, 0x01, 0x48,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x69,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x21,
      0x97, 0x02, 0xF0, 0x10,

      0x98,
    ],
  },

  "UART MicroLink": {
    description: "CPU writes a message through UART.",
    start: 0x1000,
    bytes: [
      0x81, 0x00, 0x00, 0x01, 0x4D,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x69,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x63,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x72,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x6F,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x4C,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x69,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x6E,
      0x97, 0x02, 0xF0, 0x10,

      0x81, 0x00, 0x00, 0x01, 0x6B,
      0x97, 0x02, 0xF0, 0x10,

      0x98,
    ],
  },

  "Timer Demo": {
    description:
      "Configure the hardware timer and trigger a compare event.",
    start: 0x1000,
    bytes: [
      // MOV R0, 0x0A
      0x81, 0x00, 0x00, 0x01, 0x0A,

      // STORE R0, 0xF021
      0x83, 0x00, 0x00, 0x02, 0xF0, 0x21,

      // MOV R0, 0x01
      0x81, 0x00, 0x00, 0x01, 0x01,

      // STORE R0, 0xF022
      0x83, 0x00, 0x00, 0x02, 0xF0, 0x22,

      // MOV R0, 0x00
      0x81, 0x00, 0x00, 0x01, 0x00,

      // STORE R0, 0xF020
      0x83, 0x00, 0x00, 0x02, 0xF0, 0x20,

      // Let timer tick.
      0x80,
      0x80,
      0x80,
      0x80,
      0x80,
      0x80,
      0x80,
      0x80,
      0x80,
      0x80,

      0x98,
    ],
  },
};

function hex(value, width = 4) {
  return `0x${Number(value ?? 0)
    .toString(16)
    .toUpperCase()
    .padStart(width, "0")}`;
}

function sleep(ms) {
  return new Promise((resolve) =>
    setTimeout(resolve, ms)
  );
}

function Register({ label, value }) {
  return (
    <div className="register">
      <span>{label}</span>
      <strong>{value}</strong>
    </div>
  );
}

function Flag({ name }) {
  return <div className="flag">{name}</div>;
}

function App() {
  const [state, setState] = useState(null);
  const [trace, setTrace] = useState([]);

  const [memory, setMemory] = useState([]);
  const [memoryStart, setMemoryStart] = useState(0);

  const [selectedDemo, setSelectedDemo] =
    useState("NOP / HALT");

  const [loaded, setLoaded] = useState(false);
  const [busy, setBusy] = useState(false);
  const [loadingProgram, setLoadingProgram] =
    useState(false);
  const [liveRunning, setLiveRunning] =
    useState(false);

  const [error, setError] = useState("");

  const [gpioPattern, setGpioPattern] =
    useState("10100101");

  const [liveSpeed, setLiveSpeed] =
    useState(180);

  const demo = DEMOS[selectedDemo];

  async function refreshMemory() {
    try {
      const response = await fetch(
        `${API}/memory`
      );

      if (!response.ok) {
        throw new Error(
          `Memory request failed: ${response.status}`
        );
      }

      const data = await response.json();

      setMemory(data.bytes ?? []);
      setMemoryStart(
        data.start_address ?? 0
      );
    } catch (err) {
      throw err;
    }
  }

  async function refresh() {
    const [
      stateResponse,
      traceResponse,
    ] = await Promise.all([
      fetch(`${API}/state`),
      fetch(`${API}/trace`),
    ]);

    if (!stateResponse.ok) {
      throw new Error(
        `State request failed: ${stateResponse.status}`
      );
    }

    if (!traceResponse.ok) {
      throw new Error(
        `Trace request failed: ${traceResponse.status}`
      );
    }

    const nextState =
      await stateResponse.json();

    const nextTrace =
      await traceResponse.json();

    setState(nextState);
    setTrace(nextTrace.entries ?? []);

    await refreshMemory();

    return nextState;
  }

  async function loadProgram() {
    setLoadingProgram(true);
    setError("");

    try {
      const response = await fetch(
        `${API}/load`,
        {
          method: "POST",
          headers: {
            "Content-Type":
              "application/json",
          },
          body: JSON.stringify({
            start_address: demo.start,
            program: demo.bytes,
          }),
        }
      );

      const text =
        await response.text();

      if (!response.ok) {
        throw new Error(
          text ||
            `Load failed (${response.status})`
        );
      }

      setLoaded(true);

      await refresh();
    } catch (err) {
      setLoaded(false);
      setError(
        err instanceof Error
          ? err.message
          : String(err)
      );
    } finally {
      setLoadingProgram(false);
    }
  }

  async function step() {
    if (
      busy ||
      liveRunning ||
      !loaded ||
      state?.halted
    ) {
      return;
    }

    setBusy(true);
    setError("");

    try {
      const response = await fetch(
        `${API}/step`,
        {
          method: "POST",
        }
      );

      const text =
        await response.text();

      if (!response.ok) {
        throw new Error(
          text ||
            `Step failed (${response.status})`
        );
      }

      await refresh();
    } catch (err) {
      setError(
        err instanceof Error
          ? err.message
          : String(err)
      );
    } finally {
      setBusy(false);
    }
  }

  async function run() {
    if (
      busy ||
      liveRunning ||
      !loaded ||
      state?.halted
    ) {
      return;
    }

    setBusy(true);
    setError("");

    try {
      const response = await fetch(
        `${API}/run`,
        {
          method: "POST",
          headers: {
            "Content-Type":
              "application/json",
          },
          body: JSON.stringify({
            cycle_limit: 1000,
          }),
        }
      );

      const text =
        await response.text();

      if (!response.ok) {
        throw new Error(
          text ||
            `Run failed (${response.status})`
        );
      }

      await refresh();
    } catch (err) {
      setError(
        err instanceof Error
          ? err.message
          : String(err)
      );
    } finally {
      setBusy(false);
    }
  }

  async function liveRun() {
    if (
      liveRunning ||
      busy ||
      !loaded ||
      state?.halted
    ) {
      return;
    }

    setLiveRunning(true);
    setError("");

    try {
      for (let i = 0; i < 1000; i += 1) {
        const response = await fetch(
          `${API}/step`,
          {
            method: "POST",
          }
        );

        const text =
          await response.text();

        if (!response.ok) {
          throw new Error(
            text ||
              `Step failed (${response.status})`
          );
        }

        const nextState =
          await refresh();

        if (nextState.halted) {
          break;
        }

        await sleep(
          Math.max(20, liveSpeed)
        );
      }
    } catch (err) {
      setError(
        err instanceof Error
          ? err.message
          : String(err)
      );
    } finally {
      setLiveRunning(false);
    }
  }

  async function reset() {
    setBusy(true);
    setError("");

    try {
      const response = await fetch(
        `${API}/reset`,
        {
          method: "POST",
        }
      );

      const text =
        await response.text();

      if (!response.ok) {
        throw new Error(
          text ||
            `Reset failed (${response.status})`
        );
      }

      setLoaded(false);

      await refresh();
    } catch (err) {
      setError(
        err instanceof Error
          ? err.message
          : String(err)
      );
    } finally {
      setBusy(false);
    }
  }

  function buildGpioProgram(pattern) {
    const normalized =
      pattern
        .trim()
        .replace(/\s/g, "");

    if (!/^[01]{8}$/.test(normalized)) {
      throw new Error(
        "GPIO pattern must contain exactly 8 binary digits."
      );
    }

    const value =
      parseInt(normalized, 2);

    return [
      // MOV R0, value
      0x81,
      0x00,
      0x00,
      0x01,
      value,

      // OUT 0xF000
      0x97,
      0x02,
      0xF0,
      0x00,

      // HALT
      0x98,
    ];
  }

  async function applyGpioPattern() {
    setError("");
    setLoadingProgram(true);

    try {
      const program =
        buildGpioProgram(
          gpioPattern
        );

      const response = await fetch(
        `${API}/load`,
        {
          method: "POST",
          headers: {
            "Content-Type":
              "application/json",
          },
          body: JSON.stringify({
            start_address: 0x1000,
            program,
          }),
        }
      );

      const text =
        await response.text();

      if (!response.ok) {
        throw new Error(
          text ||
            "GPIO program load failed."
        );
      }

      setSelectedDemo(
        "GPIO Pattern"
      );

      setLoaded(true);

      await refresh();
    } catch (err) {
      setError(
        err instanceof Error
          ? err.message
          : String(err)
      );
    } finally {
      setLoadingProgram(false);
    }
  }

  useEffect(() => {
    refresh().catch((err) => {
      setError(
        err instanceof Error
          ? err.message
          : "Unable to connect to MicroLink API."
      );
    });
  }, []);

  const executionDisabled =
    busy ||
    liveRunning ||
    !loaded ||
    state?.halted === true;

  const gpio =
    Number(state?.gpio ?? 0);

  const binaryGpio =
    gpio
      .toString(2)
      .padStart(8, "0");

  return (
    <main className="app">
      <header className="header">
        <div>
          <h1>MicroLink</h1>
          <p>
            8-bit Embedded Computer Simulator
          </p>
        </div>

        <div className="status">
          <span
            className={
              state?.halted
                ? "status-dot halted"
                : "status-dot"
            }
          />

          {state?.halted
            ? "HALTED"
            : loaded
              ? "READY"
              : "NO PROGRAM"}
        </div>
      </header>

      {error && (
        <div className="error">
          {error}
        </div>
      )}

      <section className="program panel">
        <div className="panel-title">
          <h2>Programs</h2>
          <span>BUILT-IN DEMOS</span>
        </div>

        <div className="program-body">
          <select
            value={selectedDemo}
            onChange={(event) => {
              setSelectedDemo(
                event.target.value
              );
              setLoaded(false);
              setError("");
            }}
            disabled={
              busy ||
              liveRunning ||
              loadingProgram
            }
          >
            {Object.keys(DEMOS).map(
              (name) => (
                <option
                  key={name}
                  value={name}
                >
                  {name}
                </option>
              )
            )}
          </select>

          <div className="program-description">
            {demo.description}
          </div>

          <div className="program-info">
            <span>
              START: {hex(demo.start)}
            </span>

            <span>
              SIZE: {demo.bytes.length} BYTES
            </span>
          </div>

          <button
            onClick={loadProgram}
            disabled={
              busy ||
              liveRunning ||
              loadingProgram
            }
          >
            {loadingProgram
              ? "LOADING..."
              : "LOAD PROGRAM"}
          </button>
        </div>
      </section>

      <section className="dashboard">
        <section className="panel cpu">
          <div className="panel-title">
            <h2>CPU</h2>
            <span>8-BIT CORE</span>
          </div>

          <Register
            label="PC"
            value={hex(state?.pc)}
          />

          <Register
            label="ACCUMULATOR"
            value={hex(
              state?.accumulator,
              2
            )}
          />

          <Register
            label="STACK POINTER"
            value={hex(
              state?.stack_pointer
            )}
          />

          <Register
            label="INSTRUCTION"
            value={hex(
              state?.instruction_register,
              2
            )}
          />

          <Register
            label="HALTED"
            value={
              state?.halted
                ? "YES"
                : "NO"
            }
          />

          <h3>FLAGS</h3>

          <div className="flags">
            <Flag name="Z" />
            <Flag name="C" />
            <Flag name="N" />
            <Flag name="V" />
          </div>
        </section>

        <section className="panel trace-panel">
          <div className="panel-title">
            <h2>Execution Trace</h2>

            <span>
              {trace.length} CYCLES
            </span>
          </div>

          <div className="trace">
            {trace.length === 0 ? (
              <div className="empty">
                Load a program to begin
                execution.
              </div>
            ) : (
              trace
                .slice()
                .reverse()
                .map(
                  (entry, index) => (
                    <div
                      className="trace-entry"
                      key={`${entry.cycle}-${index}`}
                    >
                      <span>
                        #{entry.cycle}
                      </span>

                      <strong>
                        {hex(
                          entry.opcode,
                          2
                        )}
                      </strong>

                      <span>
                        {hex(
                          entry.pc_before
                        )}
                        {" → "}
                        {hex(
                          entry.pc_after
                        )}
                      </span>

                      <span>
                        A{" "}
                        {hex(
                          entry.accumulator_after,
                          2
                        )}
                      </span>
                    </div>
                  )
                )
            )}
          </div>
        </section>

        <aside className="devices">
          <section className="panel device">
            <div className="panel-title">
              <h2>GPIO</h2>
              <span>8-BIT PORT</span>
            </div>

            <div className="device-value">
              {hex(gpio, 2)}
            </div>

            <div className="binary-value">
              {binaryGpio}
            </div>

            <div className="gpio-controls">
              <label>
                CUSTOM PATTERN
              </label>

              <div className="gpio-input-row">
                <input
                  value={gpioPattern}
                  onChange={(event) =>
                    setGpioPattern(
                      event.target.value
                        .replace(
                          /[^01]/g,
                          ""
                        )
                        .slice(0, 8)
                    )
                  }
                  maxLength={8}
                  placeholder="10100101"
                />

                <button
                  onClick={
                    applyGpioPattern
                  }
                  disabled={
                    busy ||
                    liveRunning ||
                    loadingProgram
                  }
                >
                  APPLY
                </button>
              </div>
            </div>

            <div className="leds">
              {Array.from(
                { length: 8 },
                (_, index) => {
                  const bit =
                    7 - index;

                  const active =
                    (gpio &
                      (1 << bit)) !==
                    0;

                  return (
                    <div
                      key={bit}
                      className={
                        active
                          ? "led on"
                          : "led"
                      }
                    >
                      <span />
                      <small>
                        {bit}
                      </small>
                    </div>
                  );
                }
              )}
            </div>
          </section>

          <section className="panel device">
            <div className="panel-title">
              <h2>UART</h2>
              <span>
                SERIAL TERMINAL
              </span>
            </div>

            <div className="terminal">
              {state?.uart ||
                "> Waiting for serial output..."}
            </div>
          </section>

          <section className="panel device">
            <div className="panel-title">
              <h2>TIMER</h2>
              <span>
                HARDWARE TIMER
              </span>
            </div>

            <div className="timer-value">
              {hex(
                state?.timer ?? 0,
                2
              )}
            </div>

            <div className="timer-meta">
              <span>COUNTER</span>
              <strong>
                {hex(
                  state?.timer ?? 0,
                  2
                )}
              </strong>
            </div>

            <div className="timer-meta">
              <span>COMPARE</span>
              <strong>
                {hex(
                  state?.timer_compare ?? 0,
                  2
                )}
              </strong>
            </div>

            <div className="timer-meta">
              <span>ENABLED</span>
              <strong>
                {state?.timer_enabled
                  ? "YES"
                  : "NO"}
              </strong>
            </div>

            <div className="timer-meta">
              <span>IRQ</span>
              <strong
                className={
                  state?.timer_interrupt_pending
                    ? "irq pending"
                    : ""
                }
              >
                {state?.timer_interrupt_pending
                  ? "PENDING"
                  : "CLEAR"}
              </strong>
            </div>

            <div className="timer-bar">
              <div
                style={{
                  width: `${Math.min(
                    Number(
                      state?.timer ?? 0
                    ) / 255 * 100,
                    100
                  )}%`,
                }}
              />
            </div>
          </section>
        </aside>
      </section>

      <section className="panel memory">
        <div className="panel-title">
          <h2>Memory</h2>
          <span>
            64 KiB ADDRESS SPACE
          </span>
        </div>

        <div className="memory-view">
          {memory.length === 0 ? (
            <div className="empty">
              Load a program to inspect
              memory.
            </div>
          ) : (
            memory.map(
              (byte, index) => (
                <div
                  className="memory-cell"
                  key={`${memoryStart}-${index}`}
                >
                  <span>
                    {hex(
                      memoryStart +
                        index
                    )}
                  </span>

                  <strong>
                    {hex(byte, 2)}
                  </strong>
                </div>
              )
            )
          )}
        </div>
      </section>

      <section className="panel live-panel">
        <div className="panel-title">
          <h2>Live Execution</h2>
          <span>
            HUMAN-VISIBLE HARDWARE EXECUTION
          </span>
        </div>

        <div className="live-controls">
          <label>
            SPEED
          </label>

          <input
            type="range"
            min="40"
            max="500"
            step="20"
            value={liveSpeed}
            onChange={(event) =>
              setLiveSpeed(
                Number(event.target.value)
              )
            }
          />

          <span>
            {liveSpeed} ms / cycle
          </span>
        </div>
      </section>

      <footer className="controls">
        <button
          onClick={step}
          disabled={executionDisabled}
        >
          STEP
        </button>

        <button
          onClick={run}
          disabled={executionDisabled}
        >
          {busy
            ? "RUNNING..."
            : "RUN"}
        </button>

        <button
          onClick={liveRun}
          disabled={executionDisabled}
        >
          {liveRunning
            ? "LIVE RUNNING..."
            : "LIVE RUN"}
        </button>

        <button
          className="secondary"
          onClick={reset}
          disabled={
            busy ||
            liveRunning
          }
        >
          RESET
        </button>
      </footer>
    </main>
  );
}

export default App;