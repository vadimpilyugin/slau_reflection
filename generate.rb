def generate(size)
	File.open 'matrices/slau', 'w' do |file|
		file.puts size
		size.times do 
			size.times do 
				file.printf "%f ", (rand-0.5)*10
			end
			file.printf "%f\n", (rand-0.5)*10
		end
	end
end

if ARGV.size < 2
	puts "Usage: generate.rb <M> <N>"
else
	M = ARGV[0].to_i
	N = ARGV[1].to_i
	(M..N).each do |i|
		generate i
		puts `build/main`.split("\n").keep_if{|s| s =~ /Невязка/ || s =~ /Размер/}
	end
end