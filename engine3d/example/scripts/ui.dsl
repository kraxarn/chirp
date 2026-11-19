module ui

using chirp.ui

export mut time: chirp.config.TimeStats = {}
export mut sub_steps: i32 = 0
export mut camera: chirp.Camera = {}
export mut body: chirp.PhysicsBody = 0
export mut world_summary: flecs.stats.WorldSummary = {}

const engine = chirp.engine_config

const padding:    f32 = 16
const row_height: f32 = 18
const left_w:     f32 = 350

DebugWindow {
	Window: {x: left_w + (padding * 2), y: padding, w: left_w, h: 195}
	Border

	_ {
		Row: {height: row_height, columns: 1}
		OrderedChildren
		if engine.debug {
			_ { Label: {text: "- debug mode -", align: Centered | Middle} }
		}
		_ { Label: {text: "{engine.name} {engine.version}", align: Left | Middle} }

		_ {
			Row: {height: row_height, columns: 2, ratio: [0.3, 0.7]}

			Performance {
				_ { Label: {text: "FPS", align: Left | Middle} }
				_ {
					Label: {
						text: "{time.fps:<3} ({time.dt * 1000:<5.2} ms, {sub_steps} steps)",
						align: Left | Middle,
					}
				}
			}

			CameraInfo {
				_ { Label: {text: "Camera", align: Left | Middle} }
				_ {
					const position = camera.position
					Label: {
						text: "{position.x:<6.2} {position.y:<6.2} {position.z:<6.2} {position.length():<6.2}",
						align: Left | Middle,
					}
				}
				_ { Label: {text: "Target", align: Left | Middle} }
				_ {
					const target = camera.target
					Label: {
						text: "{target.x:<6.2} {target.y:<6.2} {target.z:<6.2} {target.length():<6.2}",
						align: Left | Middle,
					}
				}
			}

			PhysicsInfo {
				_ { Label: {text: "Position", align: Left | Middle} }
				_ {
					const position = body.position()
					Label: {
						text: "{position.x:<6.2} {position.y:<6.2} {position.z:<6.2} {position.length():<6.2}",
						align: Left | Middle,
					}
				}
				_ { Label: {text: "Velocity", align: Left | Middle} }
				_ {
					const velocity = body.linear_velocity()
					Label: {
						text: "{velocity.x:<6.2} {velocity.y:<6.2} {velocity.z:<6.2} {velocity.length():<6.2}",
						align: Left | Middle,
					}
				}
				_ { Label: {text: "Rotation", align: Left | Middle} }
				_ {
					const rotation = body.rotation()
					Label: {
						text: "{rotation.x:<6.2} {rotation.y:<6.2} {rotation.z:<6.2} {rotation.w:<6.2}",
						align: Left | Middle,
					}
				}
			}
		}
	}
}

if world_summary.entity_count > 0 {
	WorldWindow {
		Window: {x: left_w + (padding * 2), y: 195 + (padding * 2), w: left_w, h: 150}
		Border
		_ {
			Row: {height: row_height, columns: 2, ratio: [0.4, 0.6]}
			OrderedChildren

			_ { Label: {text: "Entities", align: Left | Middle} }
			_ { Label: {text: world_summary.entity_count, align: Left | Middle} }

			_ { Label: {text: "Tables", align: Left | Middle} }
			_ { Label: {text: world_summary.table_count, align: Left | Middle} }

			_ { Label: {text: "Systems", align: Left | Middle} }
			_ { Label: {text: world_summary.systems_ran_frame, align: Left | Middle} }

			_ { Label: {text: "Observers", align: Left | Middle} }
			_ { Label: {text: world_summary.observers_ran_frame, align: Left | Middle} }

			_ { Label: {text: "Queries", align: Left | Middle} }
			_ { Label: {text: world_summary.queries_ran_frame, align: Left | Middle} }

			_ { Label: {text: "Commands", align: Left | Middle} }
			_ { Label: {text: world_summary.command_count_frame, align: Left | Middle} }
		}
	}
}

InputWindow {
	Window: {x: left_w + (padding * 2), y: 195 + 150 + (padding * 3), w: left_w, h: 310}
	Border
	export mut selected: i32 = 0
	Content {
		Row: {height: row_height * 1.5, columns: 1}
		OrderedChildren
		Sources {
			Combo: {
				items: ["Keyboard", "Mouse", "Gamepad buttons", "Gamepad labels", "Name map"],
				item_height: row_height,
				size: {x: 150, y: 120},
			}
		}
		if selected == 0 {
			_ { Label: {text: "Keyboard", align: Left | Middle} }
		} else if selected == 1 {
			_ { Label: {text: "Mouse", align: Left | Middle} }
		} else if selected == 2 {
			_ { Label: {text: "Gamepad buttons", align: Left | Middle} }
		} else if selected == 3 {
			_ { Label: {text: "Gamepad labels", align: Left | Middle} }
		} else if selected == 4 {
			_ { Label: {text: "Name map", align: Left | Middle} }
		}
	}
}
