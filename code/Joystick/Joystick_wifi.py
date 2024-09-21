import pygame
import socket
import sys
import math

'''
This code sends joystick commands to the receiver esp32 on the drone
through the WiFi
'''

# ESP32 WiFi details
ESP32_IP = "192.168.4.1"  # Default IP when ESP32 is in AP mode
ESP32_PORT = 4210  # The port your ESP32 is listening on

# Initialize Pygame
pygame.init()

# Set up the display
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Quadcopter Control")

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
GRAY = (200, 200, 200)

# Fonts
font = pygame.font.Font(None, 36)
small_font = pygame.font.Font(None, 24)

# UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Control variables
throttle = 0
pitch = 0
roll = 0
yaw = 0

def send_command():
    command = f"T:{throttle},P:{pitch},R:{roll},Y:{yaw}"
    try:
        sock.sendto(command.encode(), (ESP32_IP, ESP32_PORT))
    except socket.error as e:
        print(f"Error sending data: {e}")

def draw_interface():
    screen.fill(WHITE)
    
    # Draw throttle bar
    pygame.draw.rect(screen, GRAY, (50, 50, 50, 400))
    throttle_height = throttle * 4  # Scale to fit 400 pixels
    pygame.draw.rect(screen, GREEN, (50, 450 - throttle_height, 50, throttle_height))
    screen.blit(font.render("Throttle", True, BLACK), (40, 460))
    
    # Draw pitch/roll indicator
    pygame.draw.circle(screen, GRAY, (300, 250), 100)
    indicator_x = 300 + roll
    indicator_y = 250 - pitch
    pygame.draw.circle(screen, RED, (indicator_x, indicator_y), 10)
    screen.blit(font.render("Pitch/Roll", True, BLACK), (260, 360))
    
    # Draw yaw indicator
    pygame.draw.circle(screen, GRAY, (600, 250), 100, 2)
    angle = math.radians(yaw * 3.6 - 90)  # Convert yaw to angle, -90 to start at top
    end_x = 600 + 100 * math.cos(angle)
    end_y = 250 + 100 * math.sin(angle)
    pygame.draw.line(screen, BLUE, (600, 250), (end_x, end_y), 4)
    screen.blit(font.render("Yaw", True, BLACK), (585, 360))
    
    # Draw values
    screen.blit(font.render(f"Throttle: {throttle}", True, BLACK), (50, 500))
    screen.blit(font.render(f"Pitch: {pitch}", True, BLACK), (50, 540))
    screen.blit(font.render(f"Roll: {roll}", True, BLACK), (300, 500))
    screen.blit(font.render(f"Yaw: {yaw}", True, BLACK), (300, 540))
    
    # Draw instructions
    instructions = [
        "Up/Down: Throttle Up/Down",
        "Left/Right: Yaw Left/Right",
        "W/S: Pitch Forward/Back",
        "A/D: Roll Left/Right",
        "Space: Emergency Stop",
        "Q: Quit"
    ]
    for i, instruction in enumerate(instructions):
        text = small_font.render(instruction, True, BLACK)
        screen.blit(text, (550, 450 + i * 25))

    pygame.display.flip()

def emergency_stop():
    global throttle, pitch, roll, yaw
    throttle = 0
    pitch = 0
    roll = 0
    yaw = 0
    send_command()
    print("EMERGENCY STOP ACTIVATED")

running = True
clock = pygame.time.Clock()

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    
    keys = pygame.key.get_pressed()
    
    # Throttle control (now with arrow keys)
    if keys[pygame.K_UP]:
        throttle = min(throttle + 1, 100)
    if keys[pygame.K_DOWN]:
        throttle = max(throttle - 1, 0)
    
    # Yaw control (now with arrow keys)
    if keys[pygame.K_RIGHT]:
        yaw = min(yaw + 1, 100)
    if keys[pygame.K_LEFT]:
        yaw = max(yaw - 1, -100)
    
    # Pitch control (now with W/S, auto-center)
    if keys[pygame.K_w]:
        pitch = min(pitch + 2, 100)
    elif keys[pygame.K_s]:
        pitch = max(pitch - 2, -100)
    else:
        pitch = int(pitch * 0.8)  # Gradually return to zero
    
    # Roll control (now with A/D, auto-center)
    if keys[pygame.K_d]:
        roll = min(roll + 2, 100)
    elif keys[pygame.K_a]:
        roll = max(roll - 2, -100)
    else:
        roll = int(roll * 0.8)  # Gradually return to zero
    
    # Emergency stop
    if keys[pygame.K_SPACE]:
        emergency_stop()
    
    # Quit
    if keys[pygame.K_q]:
        running = False
    
    send_command()
    draw_interface()
    clock.tick(30)  # 30 FPS

pygame.quit()
sock.close()
sys.exit()