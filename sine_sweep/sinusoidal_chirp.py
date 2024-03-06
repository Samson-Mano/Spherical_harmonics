import matplotlib.pyplot as plt
import math as math
import numpy as np

class sinusoidal_ChirpAnalysis:
    def __init__(self, f_start, f_end, f_m, f_phase, t_final, t_delta):
        # Initialize the data
        self.a_param = (f_end + f_start) * 0.5
        self.b_param = (f_end - f_start) * 0.5
        self.omega_m = 2.0 * math.pi * f_m # frequency of the sine function
        self.f_phase = f_phase # phase of the sine function
        self.t_final = t_final
        self.t_delta = t_delta
        
        self.t_data = np.arange(0.0, t_final + t_delta, t_delta)

    def sinusoidal_chirp_freq(self):
        # Chirp frequencies    

        chirp_freq = np.zeros_like(self.t_data)
        for i, t in enumerate(self.t_data):
                chirp_freq[i] = self.a_param + (self.b_param * (math.sin((self.omega_m * t) - self.f_phase)))

        return chirp_freq

    def sinusoidal_chirp_signal(self):
        # Calculate the frequency sweep as a sinusoidal function

        chirp_phi = np.zeros_like(self.t_data)
        for i, t in enumerate(self.t_data):
                chirp_phi[i] = (self.a_param * t) + (self.b_param * (math.cos(self.f_phase) - math.cos(self.omega_m * t - self.f_phase)) * (1.0/self.omega_m))

        sinu_chirp = np.sin(2 * np.pi * chirp_phi)
        return sinu_chirp

# Plot the chirp frequencies
f_start = 1.0
f_end = 5.0
t_delta = 0.001
t_final = 30
f_m = 0.1
f_phase = math.pi*0.5

sinu_chirp = sinusoidal_ChirpAnalysis(f_start, f_end,f_m, math.pi*0.0, t_final, t_delta)
sinu_chirp1 = sinusoidal_ChirpAnalysis(f_start, f_end,f_m, math.pi*0.5, t_final, t_delta)

chirp_result = np.zeros_like(np.arange(0.0, t_final + t_delta, t_delta))

phase_data = np.arange(-2.0, 2.0, 0.01)

for i, p_d in enumerate(phase_data):
    chirp_result = chirp_result + sinusoidal_ChirpAnalysis(f_start, f_end,f_m, math.pi*p_d, t_final, t_delta).sinusoidal_chirp_signal()


# Create a figure and 2 subplots
fig, axes = plt.subplots(nrows=2, ncols=1, figsize=(20, 8))

# Plot data on each subplot
axes[0].plot(sinu_chirp.t_data, sinu_chirp.sinusoidal_chirp_freq(), color='blue')
axes[0].plot(sinu_chirp.t_data, sinu_chirp1.sinusoidal_chirp_freq(), color='orange')
axes[0].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[0].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[0].set_title('Chirp Frequencies')

# axes[1].plot(sinu_chirp.t_data, sinu_chirp.sinusoidal_chirp_signal() + sinu_chirp1.sinusoidal_chirp_signal(), color='green')
axes[1].plot(sinu_chirp.t_data, chirp_result, color='green')
# axes[1].plot(sinu_chirp.t_data, sinu_chirp1.sinusoidal_chirp_signal(), color='red')
axes[1].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[1].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[1].set_title('Chirp Signal')

axes[-1].set_xlabel('Time (sec)')

# Set the main title for the figure
fig.suptitle('Sinusoidal Chirp')

# Display the plot
plt.show()