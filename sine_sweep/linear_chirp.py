import matplotlib.pyplot as plt
import numpy as np

class linear_ChirpAnalysis:
    def __init__(self, f_start, f_end, t_final, t_delta):
        # Initialize the data
        self.f_start = f_start
        self.f_end = f_end
        self.t_final = t_final
        self.t_delta = t_delta
        
        # Chirp rate
        self.c_rate = (f_end - f_start) / float(t_final)

        self.t_data = np.arange(0.0, t_final + t_delta, t_delta)

    def linear_chirp_freq(self):
        # Chirp frequencies    
        return self.f_start + self.c_rate * self.t_data

    def linear_chirp_signal(self):
        chirp_freq = self.c_rate * 0.5 * np.square(self.t_data) + self.f_start * self.t_data

        lin_chirp = np.sin(2 * np.pi * chirp_freq)
        return lin_chirp

# Plot the chirp frequencies
f_start = 1.0
f_end = 5.0
t_delta = 0.001
t_final = 10

lin_chirp = linear_ChirpAnalysis(f_start, f_end, t_final, t_delta)
    
# Create a figure and 2 subplots
fig, axes = plt.subplots(nrows=2, ncols=1, figsize=(20, 8))

# Plot data on each subplot
axes[0].plot(lin_chirp.t_data, lin_chirp.linear_chirp_freq(), color='blue')
axes[0].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[0].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[0].set_title('Chirp Frequencies')

axes[1].plot(lin_chirp.t_data, lin_chirp.linear_chirp_signal(), color='green')
axes[1].axhline(y=0.0, color='black', linestyle='-', linewidth=1)
axes[1].axvline(x=0.0, color='black', linestyle='-', linewidth=1)
axes[1].set_title('Chirp Signal')

axes[-1].set_xlabel('Time (sec)')

# Set the main title for the figure
fig.suptitle('Linear Chirp')

# Display the plot
plt.show()