# Global variables get from parameters log file
THD = 50  # Dark threshold
THB = 500  # Bright threshold

def optimal_action(occupancy, light_level, switch_position):
    # Define the table with predefined optimal actions
    # Format: (Occupancy, Light Level, Switch Position) -> Optimal Action
    table = {
        (False, 'Dark', 0): 0,
        (False, 'Dark', 0.5): 0,
        (False, 'Dark', 1): 0,
        (False, 'Comfort', 0): 0,
        (False, 'Comfort', 0.5): 0,
        (False, 'Comfort', 1): 0,
        (False, 'Bright', 0): 0,
        (False, 'Bright', 0.5): 0,
        (False, 'Bright', 1): 0,
        (True, 'Dark', 0): 0.5,
        (True, 'Dark', 0.5): 1,
        (True, 'Dark', 1): 1,
        (True, 'Comfort', 0): 0,
        (True, 'Comfort', 0.5): 0.5,
        (True, 'Comfort', 1): 1,
        (True, 'Bright', 0): 0,
        (True, 'Bright', 0.5): 0,
        (True, 'Bright', 1): 0.5
    }

    # Determine light level category
    if light_level < THD:
        light_level_category = 'Dark'
    elif light_level > THB:
        light_level_category = 'Bright'
    else:
        light_level_category = 'Comfort'

    # Lookup the optimal action in the table
    optimal_action = table.get((occupancy, light_level_category, switch_position))

    return optimal_action

# Example usage:
occupancy = True
light_level = 200
switch_position = 0.5
print("Optimal action:", optimal_action(occupancy, light_level, switch_position))
